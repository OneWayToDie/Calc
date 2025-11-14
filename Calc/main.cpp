#include<Windows.h>
#include<string>
#include<sstream>
#include<cmath>
#include"resource.h"

HWND hDisplay;                                      //Поле ввода калькулятора
double number_one = 0, number_two = 0, result = 0;  //Первое вводимое число, второе и результат
char operation = '\0';                              //Текущая операция, L'0' - пустой символ
bool newNumber = true;                              //Проверка на начало ввода
std::string displayText = "0";                      //Текст на поле ввода
std::string expression = "";                        //Выражение
bool hasSquareRoot = false;                         //Проверка на наличие квадратного корня в выражении

void Calc_Function(char digit);                     //Обработка нажатия кнопок
void FullExpression();                              //Обновление дисплея
void SquareRoot();                                  //Функция для вычисления квадратного корня
void SetOperation(char op);                         //Выбор операции
void Calculate();                                   //Вычисление
void Clear();                                       //Очистка
BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);   //Обработка диалогового окна

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    setlocale(LC_ALL, "");
    DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG_CALCULATOR), NULL, (DLGPROC)DlgProc, 0);
    return 0;
}

void Calc_Function(char digit)
{
    if (newNumber)
    {
        displayText = digit;    //Замена нуля на введённую цифру
        newNumber = false;      //Сбрасываю новое число

        if (operation != '\0') //Если операция выбрана
        {
            if (hasSquareRoot)
            {
               //Если есть квадратный корень в выражении:
               expression = std::to_string((int)number_one) + " " + operation + "Sqrt" + displayText;
            }
            else
            {
                expression = std::to_string((int)number_one) + " " + operation + " " + displayText; //Если нет кв. корня - обычное выражение
            }
        }
        else
        {
            expression = "";        //В противном случае очищаю
        }
    }
    else                            //При продолжении ввода числа
    {
        if (displayText == "0")
            displayText = digit;    //Замена нуля на число
        else
            displayText += digit;   //добавление числа к текущему
        if (operation != '\0')      //При выбранной операции - обновляю выражение
        {
            if (hasSquareRoot)
            {
                expression = std::to_string((int)number_one) + " " + operation + "Sqrt" + displayText;
            }
            else
            {
                expression = std::to_string((int)number_one) + " " + operation + " " + displayText;
            }
        }
    }
    FullExpression();               //Изменяем вывод на дисплее
}

void FullExpression()
{
    //Если выражение пустое - показываю displayText, или само выражение
    SetWindowText(hDisplay, expression.empty() ? displayText.c_str() : expression.c_str()); //SetWindowText - отвечает за показ текста в "главном окне"
}

void SquareRoot()
{
    if (!newNumber && operation != '\0')
    {
        hasSquareRoot = true; //ставлю значение на наличие квадратного корня

        // Обновляю выражение
        expression = std::to_string((int)number_one) + " " + operation + "Sqrt";
        FullExpression();

        // Сбрасываю дисплей для ввода числа под корнем
        displayText = "0";
        newNumber = true;
    }
    else if (newNumber && operation != L'\0' && !hasSquareRoot) // Если операция выбрана, но число еще не введено
    {
        hasSquareRoot = true;
        expression = std::to_string((int)number_one) + " " + operation + "Sqrt";
        FullExpression();
    }
}

void SetOperation(char op)
{
    if (!newNumber) //Если число введено
    {
        number_one = std::stod(displayText.c_str(), nullptr);   //Преобразую текст в число
        operation = op;                                         //Сохраняю операцию
        newNumber = true;                                       //Готовность в вводу второго числа
        hasSquareRoot = false;                                  //Квадратного корня нема
        expression = displayText + " " + operation + " ";       //Показываю число, операцию и второе число
        FullExpression();
    }
    else if (operation != L'\0')                                //Если операция уже была выбрана
    {
        operation = op;                                         //Меняю операцию
        expression = std::to_string((int)number_one) + " " + operation + " ";
        if (hasSquareRoot)
        {
            expression += "Sqrt";                               //Если кв. корень был - сохраняю его в выражении
        }
        FullExpression();
    }
}

void Calculate()
{
    if (!newNumber && operation != L'\0')                       //Если есть второе число и операция
    {
        number_two = std::stod(displayText.c_str(), nullptr);   //Преобразую второе число

        if (hasSquareRoot)                      
        {
            if (number_two >= 0)
            {
                number_two = std::sqrt(number_two);
            }
            else
            {
                expression = "Число отрицательное, не могу вычислить корень";
                FullExpression();
                return;
            }
        }

        std::string ourExpression = expression;                 //Сохраняю выражение

        switch (operation)                                      //Выполняю выбранную операцию
        {
        case '+': result = number_one + number_two; break;
        case '-': result = number_one - number_two; break;
        case '*': result = number_one * number_two; break;
        case '/':
            if(number_two != 0)
                result = number_one / number_two;
            else
            {
                expression = "На ноль делить нельзя";
                FullExpression();
                return;                                           //При делении на ноль выхожу из программы  
            }
            break;
        }
        std::stringstream ss;
        ss << result;                                              //Преобразую результат в строку
        expression = ourExpression + " = " + ss.str();             
        FullExpression();
        displayText = ss.str();                                    //Сохраняю результат как текущее число
        expression = "";                                           //Очищаю выражение(показываю результат)
        number_one = result;                                       //Сохраняю результат для дальнейшей работы с ним
        operation = '\0';                                          //Сбрасываю операцию
        newNumber = false;                                         //Готовность к вводу нового числа
        FullExpression();
    }
}

void Clear()
{
    number_one = number_two = result = 0;                           //Обнуляю числа
    operation = '\0';                                              //Сбрасываю операцию
    displayText = "0";                                             //Сбрасываю дисплей
    newNumber = true;                                               //Готовность к вводу нового числа
    FullExpression();                                               //Обновляю дисплей до 0
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) //DlgProc - Dialog Procedure - обработчик далогового окна, hwnd - указатель на окно
//msg - отвечает за тип сообщения, wParam и lParam - отвечают за доп информацию об объекте
{
    switch (msg)                //Обработка разных типов сообщений
    {
    case WM_INITDIALOG:         //Сообщение инициализации диалога(WM_INITDIALOG - настраивает элементы перед показом окна)
    {
        hDisplay = GetDlgItem(hwnd, IDC_EDIT_DISPLAY);  //указатель на поле ввода
        
        //Создаю шрифт
        HFONT hFont = CreateFont
        (20,                    //Высота шрифта
         0,                     //Ширина( 0 = автоматическому выбору)
         0,                     //Угол наклона
         0,                     //Угол ориентации
         FW_NORMAL,             //Толщина
         FALSE,                 //Курсив(True = да, False = нет)
         FALSE,                 //Подчёркивание
         FALSE,                 //Зачёркивание
         DEFAULT_CHARSET,       //Кодировка
         OUT_DEFAULT_PRECIS,    //Точность вывода
         CLIP_DEFAULT_PRECIS,   //Качество
         DEFAULT_QUALITY,       //Шаг и семейство
         DEFAULT_PITCH, "Times New Roman" //Название шрифта
        );
        SendMessage(hDisplay, WM_SETFONT, (WPARAM)hFont, TRUE);
    }
    return TRUE;

    case WM_COMMAND:            //Сигнал от кнопок(WM_COMMAND - Window Message - COMMAND from control)
    {
        switch (LOWORD(wParam)) //Определяю какая кнопка нажата(LOWORD(wParam) - в данном случае определяет id элемента(LOWer WORD of wParam))
        {
            //Обработка кнопок
        case IDC_BUTTON_ONE: Calc_Function('1'); break; //IDC_BUTTON - айди кнопок цифр и привязка к ним
        case IDC_BUTTON_TWO: Calc_Function('2'); break;
        case IDC_BUTTON_THREE: Calc_Function('3'); break;
        case IDC_BUTTON_FOUR: Calc_Function('4'); break;
        case IDC_BUTTON_FIVEE: Calc_Function('5'); break;
        case IDC_BUTTON_SIXX: Calc_Function('6'); break;
        case IDC_BUTTON_SEVEN: Calc_Function('7'); break;
        case IDC_BUTTON_EIGHT: Calc_Function('8'); break;
        case IDC_BUTTON_NINE: Calc_Function('9'); break;
        case IDC_BUTTON_ZERO: Calc_Function('0'); break;

            //Обработка Операций
        case IDC_BUTTON_ADDITION: SetOperation('+'); break; //Айди кнопок операций и привязка операций к кнопкам
        case IDC_BUTTON_SUBTRACTION: SetOperation('-'); break;
        case IDC_BUTTON_MULTIPLICATION: SetOperation('*'); break;
        case IDC_BUTTON_DIVISION: SetOperation('/'); break;

            //Обработка специальных кнопок
        case IDC_BUTTON_EQUALS: Calculate(); break; //Айди специальных кнопок и привязка их к кнопкам
        case IDC_BUTTON_DELETE: Clear(); break;
        case IDC_BUTTON_SQRT: SquareRoot(); break;

        case IDCANCEL:
            EndDialog(hwnd, 0); //Закрываю диалоговое окно калькулятора
            return TRUE;
        }
    }
    break;
    case WM_CLOSE:  //Window Message - ClOSE window, закрываем окно.
        EndDialog(hwnd, 0); //Сообщаю программе о закрытии окна
        return TRUE;
    }

    return FALSE;
}
