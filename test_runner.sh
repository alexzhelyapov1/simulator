#!/usr/bin/env bash

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
TEST_DIR="$SCRIPT_DIR/test"
ANSWERS_DIR="$TEST_DIR/test_answers"
BUILD_DIR="./build"
SIMULATOR_PATH="$BUILD_DIR/Simulator"

echo "Начинаю сборку проекта..."
cd "$BUILD_DIR"
ninja all
echo "Сборка проекта прошла успешно."
cd "$SCRIPT_DIR"


find "$TEST_DIR" -maxdepth 1 -name "*.s" -print0 | while IFS= read -r -d $'\0' s_file; do
  
  
  s_file_name=$(basename "$s_file")
  echo "Запускаю тест для файла: $s_file_name"
  
  
  output_file_name="${s_file_name%.s}.elf" 
  elf_file="$BUILD_DIR/$output_file_name"
  answer_file="${ANSWERS_DIR}/${s_file_name%.s}"




"$SCRIPT_DIR/compile-elf.sh" "$s_file" "$BUILD_DIR" "$output_file_name"
tmp_file=$(mktemp)
"$SIMULATOR_PATH" "$elf_file" >> "$tmp_file" 2>&1

 if [ ! -f "$answer_file" ]; then
        echo -e "${RED}Нет файла с ответом для $s_file_name в $ANSWERS_DIR/ Проверьте, что он есть${NC}"
        rm -f "$tmp_file"
        continue
    fi
    

    filtered_answer=$(grep -v "Sim duration" "$answer_file")
    filtered_tmp=$(grep -v "Sim duration" "$tmp_file")


    echo "Сравниваю с ответом"
    if diff -q <(echo "$filtered_tmp") <(echo "$filtered_answer") > /dev/null; then
        echo -e "${GREEN}Тест для $s_file_name пройден!${NC}"
    else
        echo -e "${RED}Тест для $s_file_name провален! Вывод не соответствует ожидаемому.${NC}"
        echo "Ожидаемый вывод (из $answer_file):"
        cat "$answer_file"
        echo "Фактический вывод (из $tmp_file):"
        cat "$tmp_file"
     fi

    rm -f "$tmp_file"

done



