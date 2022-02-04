#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

typedef struct detail // деталь
{
    int width;
    int length;
    int quantity;
} detail;

typedef struct remnant //обрезок
{
    int x;
    int y;
    int w;
    int l;
    int squre;
}remnant;

typedef struct section //разрез
{
    int x;
    int y;
    int diretion;//0 горизонтально, 1 вертикально
}section;

class Order {
public:
    int w_list;//ширина исходного листа
    int l_list;//длина исходного листа
    std::vector<detail> details;//заказаные детали
    std::vector<remnant> remnants; //обрезки на текущий осмотр
    std::vector<section> sections; //разрезы на текущий осмотр
    static std::vector<section> res_sections; //выгодные разрезы
    static int res_squre; //выгодная площадь остатков

    Order(int w, int l, std::vector<detail> z_details);
    void chek_variants(int start, int end, int first_cut);
    void guillotine(int num_det, int q, int num_rem, int first_cut, std::vector<detail> i_det, std::vector<remnant> i_rem, std::vector<section> i_sec, int quant_list);
    
};

void init_rem(int nx, int ny, int nw, int nl, remnant *rem);
int q_metric (detail det, remnant rem); // выбор расположения детали (с / без поворота)
int read_order(int *w_map, int *l_map, std::vector<detail> *details);
void print_result (int *res, int count_for_print, string name_output);