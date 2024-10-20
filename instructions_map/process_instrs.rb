require 'psych'
require 'erb'

class InstructionSet
  def initialize(yaml_file)
    @yaml_data = Psych.load_file(yaml_file)
  end

  def find_by_format(format)
    matching_instructions = {}

    @yaml_data['instructions'].each do |instruction_name, details|
      if details['format'] == format
        matching_instructions[instruction_name] = details
      end
    end
    matching_instructions
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

  def get_args(format)
    format_info = get_format_info(format)
    format_args = {}
    format_info.each do |arg_name, range|
      start_bit, end_bit = range
      mask = ((1 << (end_bit - start_bit + 1)) - 1) << start_bit
      format_args[arg_name] = start_bit, mask
    end
    format_args
  end
end

template = ERB.new(File.read('generate_insts.erb')) 

result = template.result(binding) 
File.write("generated_functions.cpp", result)
