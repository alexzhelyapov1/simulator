require 'psych'
require 'erb'

class InstructionSet
  def initialize(yaml_file)
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

  def get_bit(number, bit_position)
    mask = 1 << bit_position
    (number & mask) > 0
  end
  
  def calculate_mask(start_bit, end_bit)
    (1 << (end_bit - start_bit + 1) - 1) << start_bit
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
        type: determine_argument_type(arg_name, format) # Add type determination
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

  def generate_opcode_mask_map
    max_opcodes = 127
    opcode_map = Array.new(max_opcodes, 0)
    @yaml_data['instructions'].each do |name, details|
      key = create_instruction_key(details)
      opcode = details['opcode']
      opcode_map[opcode] = key
    end
    opcode_map
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

instruction_set = InstructionSet.new('riscv_instructions.yaml')
instruction_set.generate_c_code('generate_insts.erb', 'generated_functions_new.cpp')
