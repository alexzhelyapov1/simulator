.global _start
_start:
    # Определяем константы для регистров
    .equ MSTATUS, 4
    
    # Тестирование CSRRSI
    addi x1, x0, 2 #  устанавливаемый бит 
    csrrsi x2, MSTATUS, 2 #Читаем mstatus, выставляем бит 2, старое значение записываем в x2

    # Проверка
    csrr x2, MSTATUS #проверяем значение после выставления бита
    
    # Выход
    addi x7, x0, 93
    sret
