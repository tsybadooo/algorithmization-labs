/*****************************************************/
/* lab_work_02_03.                                   */
/*  Введення/виведення даних різних типів засобами   */
/*  мови С++ з використанням маніпуляторів.          */
/*****************************************************/
#include <iostream>
#include <iomanip>
#include "../common_console.h"
using namespace std;

int main()
{
    console_utf8();
    char ac = 'A', bc = 'B', cc, dc;
    int ai = 18, bi = -18, ci, di;
    unsigned int au = 18, bu = 24, cu, du;
    float af = 12.34567f, bf = 12.234567f, cf, df;

    // Символьний тип
    cout << "\n Введіть значення змінних cc та dc символьного типу:" << endl;
    cin >> cc >> dc;
    cout << "\n Значення змінної ac=" << setw(10) << right << ac << endl;
    cout << " Значення змінної bc=" << setw(10) << left  << bc << endl;
    cout << " Значення змінної cc=" << setw(10) << right << cc << endl;
    cout << " Значення змінної dc="; cout.width(10); cout << left << dc << endl;

    // Цілий тип зі знаком
    cout << "\n Введіть значення змінних ci та di цілого типу зі знаком:" << endl;
    cin >> ci >> di;
    cout << "\n Значення змінної ai=" << setw(10) << right << ai << endl;
    cout << " Значення змінної bi=" << setw(10) << right << bi << endl;
    cout << " Значення змінної ci=" << setw(10) << left  << ci << endl;
    cout << " Значення змінної di=" << setw(10) << left  << di << endl;

    // Цілий тип без знаку - у 10-й, 8-й і 16-й системах
    cout << "\n Введіть значення змінних cu та du цілого типу без знаку:" << endl;
    cin >> cu >> du;
    unsigned int uv[4] = {au, bu, cu, du};
    const char *un[4] = {"au", "bu", "cu", "du"};
    for (int i = 0; i < 4; i++) {
        cout << "\n Значення змінної " << un[i] << "=\n"
             << setw(5) << dec << right << uv[i] << " - в 10-й нотації\n"
             << setw(5) << oct << right << uv[i] << " - у 8-й нотації\n"
             << setw(5) << hex << right << uv[i] << " - в 16-й нотації" << dec << endl;
    }

    // Дійсний тип
    cout << "\n Введіть значення змінних cf та df дійсного типу:" << endl;
    cin >> cf >> df;
    cout << setprecision(4);
    cout << "\n Значення змінної af=" << right << fixed << af << endl;
    cout << " Значення змінної bf=" << right << fixed << bf << endl;
    cout << " Значення змінної cf=" << left << scientific << cf << endl;
    cout << " Значення змінної df=" << left << scientific << df << endl;
    cout << "\n Значення змінної af=" << setw(12) << showpoint << setfill('X') << right << fixed << af << endl;
    cout << " Значення змінної af=" << setw(12) << showpoint << setfill('X') << left  << fixed << af << endl;
    cout << " Значення змінної af=" << setw(12) << showpoint << setfill('X') << right << scientific << af << endl;
    cout << " Значення змінної af=" << setw(12) << showpoint << setfill('X') << left  << scientific << af << endl;
    return 0;
}
