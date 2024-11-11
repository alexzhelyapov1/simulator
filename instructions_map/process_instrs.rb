require 'psych'
require 'erb'
require 'optparse'

class InstructionSet
  YAML_PATH = ""

  def initialize(yaml_file)
    unless File.exist?(yaml_file)
      puts "Error: riscv_instructions.yaml not found at #{yaml_file}"
      exit 1
    end
    @yaml_data = Psych.load_file(yaml_file)
    @instruction_definitions = group_instructions_by_format
  end

  def group_instructions_by_format
    @yaml_data['instructions'].group_by { |_, details| details['format'] }
  end

  def instruction_definitions
    @instruction_definitions
  end

  def find_by_format(format)
    @yaml_data['instructions'].select { |_, details| details['format'] == format }
  end

  def get_format_info(format)
    format_info = {}
    @yaml_data['formats'].each do |format_name, details|
      if format_name == format
        format_info = details
      end
    end
    format_info
  end
  
  def calculate_mask(start_bit, end_bit)
    ((1 << (end_bit - start_bit + 1)) - 1) << start_bit
  end

  def set_bits(value, start_bit)
    value << start_bit
  end

  def get_args(format)
    format_info = get_format_info(format)
    format_info.each_with_object({}) do |(arg_name, range), format_args|
      start_bit, end_bit = range
      mask = calculate_mask(start_bit, end_bit)
      format_args[arg_name] = {
        start_bit: start_bit,
        end_bit: end_bit,
        mask: mask,
        type: determine_argument_type(arg_name, format)
      }
    end
  end

  def determine_argument_type(arg_name, format)
    case arg_name
    when 'rd', 'rs1', 'rs2'
      'int'
    when 'imm', 'offset'
      'int64_t'
    when 'shamt'
      'int'
    else
      'int'
    end
  end

  def generate_c_code(template_path, output_path)
    template = ERB.new(File.read(template_path))
    result = template.result(binding)
    File.write(output_path, result)
  end

  def generate_instruction_keys
    @yaml_data['instructions'].each_with_object({}) do |(instruction_name, details), instruction_map|
      key = create_instruction_key(details)
      instruction_map[key] = instruction_name
    end
  end

  def generate_map
    instruction_map = generate_instruction_keys
    cpp_map_entries = instruction_map.map do |key, instruction_name|
      "{#{key}, #{instruction_name}},"
    end.join("\n")
    cpp_map_entries
  end

 # Получаю для соотв формата общую маску для спец полей
  def create_general_mask(opcode, format)
    format_details = get_format_info(format)
    instructions_with_format = find_by_format(format)
    fields = ['f3', 'f7', 'opcode']
    if instructions_with_format['opcode'] == opcode && instructions_with_format.key?('shamt')
      fields << 'shamt'
    end
    mask = fields.inject(0) do |mask, field|
      mask | (format_details.key?(field) ? calculate_mask(*format_details[field]) : 0)
    end
    mask
  end
  
  # Get mask for field
  def create_specific_mask(format, field_name)
    format_details = get_format_info(format)
    return { mask: 0, shift: 0 } unless format_details.key?(field_name) 
    start_bit, end_bit = *format_details[field_name]
    mask = calculate_mask(start_bit, end_bit)
    {mask: mask, shift: start_bit }
  end

  # Gen mask general or specific
  def create_mask_format_identifier(opcode, format, id)
    if id == 'general'
      create_general_mask(opcode, format)
    else
      create_specific_mask(format, id)
    end
  end

  # Генережка массива из масок и сдвигов + указатель на функцию getImm
  # 1. получили опкод -> узнали формат инструкции
  def get_format_from_opcode(opcode)
    instruction_details = @yaml_data['instructions'].detect { |name, details| details['opcode'] == opcode }&.last
    instruction_details&.[]('format')
  end
  
  # 2. по формату инструкции поняли какую ф-ю getImm брать
  def how_get_imm(format)
    case format
    when 'I'
      'getImmI'
    when 'R'
      'getImmInvalid'
    when 'B'
      'getImmB'
    when 'S'
      'getImmS'
    when 'U'
      'getImmU'
    when 'J'
      'getImmJ'
    end
  end

  # 3. по формату -> генерим array из масок и сдвигов для rd, rs1, rs2 если есть
  def generate_mask_shift_arr(opcode)
    format = get_format_from_opcode(opcode)
    format_details = get_format_info(format)
    mask_info_array = []
    mask_info_array << create_mask_format_identifier(opcode, format, 'general')
    ['rd', 'rs1', 'rs2'].each do |field|
      if format_details.key?(field) 
        mask_data = create_specific_mask(format, field)
        mask_info_array << mask_data[:mask]
        mask_info_array << mask_data[:shift]
      else  
        mask_info_array << 0 << 0
      end
    end
    mask_info_array
  end

  # 4. собираем массив по опкодам от 0 до 127
  def generate_decode_map
    decode_map = []
    (0...127).each do |opcode|
      format = get_format_from_opcode(opcode)
      if format.nil?  
        decode_map << [[0, 0, 0, 0, 0, 0, 0], 'getImmInvalid'] 
      else
        mask_shift_array = generate_mask_shift_arr(opcode)
        get_imm_func = how_get_imm(format)
        
        mask_array = Array.new(7,0)
        mask_shift_array.each_with_index{|item,i| mask_array[i] = item}
        decode_map << [mask_array, get_imm_func]
      end
    end
    decode_map
  end
  
  def create_instruction_key(details)
    format_details = @yaml_data['formats'][details['format']]
    key = 0
    ['opcode', 'f3', 'f7', 'shamt'].each do |field|
      next unless details.key?(field) && format_details.key?(field)
      start_bit, end_bit = format_details[field]
      value = details[field]
      key |= set_bits(value, start_bit)
    end
    key
  end

end

options = {}
OptionParser.new do |opts|
  opts.banner = "Usage: script.rb [options]"

  opts.on("-sd DIR", "--sourse_dir DIR", "dir where riscv_instructions.yaml, generate_insts.erb") do |dir|
    options[:dir] = dir
  end

  opts.on("-gen GEN_DIR", "--gen_dir GEN_DIR", "dir where gen_func.h") do |gen|
    options[:gen] = gen
  end

  print opts, "\n"
end.parse!

unless options[:dir] && options[:gen]
  puts options
  exit 1
end

print options[:dir], "\n", options[:gen], "\n"
InstructionSet::YAML_PATH = File.join(options[:dir], 'riscv_instructions.yaml')
print InstructionSet::YAML_PATH, "\n"
instruction_set = InstructionSet.new(InstructionSet::YAML_PATH)
instruction_set.generate_c_code(options[:dir] + 'generate_insts.erb', options[:gen] + 'gen_func.h')
