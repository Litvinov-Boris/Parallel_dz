#include "guillotine.h"

static int res_squr(std::vector<remnant> rem);


int read_order(int *w_map, int *l_map, std::vector<detail> *details)
{
    std::string line;
    detail new_det;
    int num_det;
    std::ifstream in("input");
    if (in.is_open())
    {
        getline(in, line);
        *w_map = atoi(line.c_str());
        getline(in, line);
        *l_map = atoi(line.c_str());
        getline(in, line);
        num_det = atoi(line.c_str());
        for (int i = 0; i < num_det; i++)
        {
            getline(in, line);
            new_det.width = atoi(line.c_str());
            getline(in, line);
            new_det.length = atoi(line.c_str());
            if ((new_det.length > *l_map || new_det.width > *w_map) && (new_det.length > *w_map || new_det.width > *l_map)){
                printf ("невозможно, деталь больше листа\n");
                in.close();
                return 0;
            }
            getline(in, line);
            new_det.quantity = atoi(line.c_str());
            details->push_back(new_det);
        }
        in.close();
        return 1;
    }
    return 0;
}
void print_result (int *res, int count_for_print, string name_output)
{
    std::ofstream out (name_output, ios::out);
    out<<"площадь обрезков = "<<res[0]<<"\n";
    int i = 1;
    while (i < count_for_print)
    {
        out<<"x = "<<res[i]<<", ";
        i++;
        out<<"y = "<<res[i]<<", ";
        i++;
        out<<"diretion - ";
        if (res[i] == 0)
            out<<"horizontally\n";
        else
            out<<"vertically\n";
        i++;
    }
}

std::vector<section>  Order::res_sections = {}; //выгодные разрезы
int Order::res_squre = 0;

Order::Order(int w, int l, std::vector<detail> z_details)
{
    w_list = w;
    l_list = l;
    details = z_details;
    remnant rem;
    init_rem(0,0,w,l,&rem);
    remnants.push_back(rem);
    sections = {};
}

void init_rem(int nx, int ny, int nw, int nl, remnant *rem)
{
    rem->x = nx;
    rem->y = ny;
    rem->w = nw;
    rem->l = nl;
    rem->squre = rem->w * rem->l;
}

int q_metric (detail det, remnant rem) // 0 - без поворота, 1 - с поворотом
{
    int u1 = det.quantity <= (rem.w / det.width) ? det.quantity : (rem.w/det.width);
    if (u1 < 1)
        return 1;
    int u2 = det.quantity <= (rem.w / det.length) ? det.quantity : (rem.w / det.length);
    if (u1 < 1)
        return 0;
    int q1 = (rem.w - det.width * u1) <= (rem.l - det.length) ? (rem.w - det.width * u1) : (rem.l - det.length);
    if (q1 < 0 )
        return 1;
    int q2 = (rem.w - det.length * u2) <= (rem.l - det.width) ? (rem.w - det.length * u2) : (rem.l - det.width);
    if (q2 < 0)
        return 0;
    return q1 <= q2 ? 0 : 1;
}

void Order:: chek_variants(int start, int end, int first_cut)
{
    int q;
    for (int i = start; i < end; i++)
    {
        q = q_metric(details[i], remnants[0]);
        guillotine(i, q, 0, first_cut, details, remnants, sections, 1);
    }
}

void Order::guillotine(int num_det, int q, int num_rem, int first_cut, std::vector<detail> i_det, std::vector<remnant> i_rem, std::vector<section> i_sec, int quant_list)
{
    int u, move_x, move_y;
    //узнать кол-во деталей в слое
    if (q == 0){
        u = i_det[num_det].quantity <= (i_rem[num_rem].w/i_det[num_det].width) ? i_det[num_det].quantity : (i_rem[num_rem].w/i_det[num_det].width);
        move_x = i_det[num_det].width;
        move_y = i_det[num_det].length;
    }else{
        u = i_det[num_det].quantity <= (i_rem[num_rem].w/i_det[num_det].length) ? i_det[num_det].quantity : (i_rem[num_rem].w/i_det[num_det].length);
        move_x = i_det[num_det].length;
        move_y = i_det[num_det].width;
    }
    //сделать 1-е 2 разреза
    section cut1, cut2;
    if (first_cut == 0){// горизонтальный разрез
        cut1.x = i_rem[num_rem].x;
        cut1.y = i_rem[num_rem].y + move_y;
        cut2.x = i_rem[num_rem].x + (move_x * u);
        cut2.y = i_rem[num_rem].y;
    } else{ // вертикальный разрез
        cut1.y = i_rem[num_rem].y;
        cut1.x = i_rem[num_rem].x + (move_x * u);
        cut2.x = i_rem[num_rem].x;
        cut2.y = i_rem[num_rem].y + move_y;
    }
    cut1.diretion = first_cut;
    cut2.diretion = first_cut == 0 ? 1 : 0;
    i_sec.push_back(cut1); i_sec.push_back(cut2);
    //разрезы для деталей на полученном куске
    cut1.diretion = 1;
    cut1.y = i_rem[num_rem].y;
    for (int i = i_rem[num_rem].x + move_x; i < i_rem[num_rem].x + (move_x * u); i += move_x)
    {
        cut1.x = i;
        i_sec.push_back(cut1);
    }
    //убрать из заказа добавленные детали
    i_det[num_det].quantity -= u;
    if (i_det[num_det].quantity == 0)
        i_det.erase(i_det.begin()+num_det);
    //добавление новых обрезков
    remnant rem;
    if (move_y != i_rem[num_rem].l)
    {
        if (first_cut == 0)
            init_rem(i_rem[num_rem].x, i_rem[num_rem].y + move_y, i_rem[num_rem].w, i_rem[num_rem].l - move_y, &rem);
        else
            init_rem(i_rem[num_rem].x, i_rem[num_rem].y + move_y, move_x * u, i_rem[num_rem].l - move_y, &rem);
        i_rem.push_back(rem);
    }
    if (move_x * u != i_rem[num_rem].w)
    {
        if(first_cut == 0)
            init_rem(i_rem[num_rem].x + move_x * u, i_rem[num_rem].y, i_rem[num_rem].w - move_x * u, move_y, &rem);
        else
            init_rem(i_rem[num_rem].x + move_x * u, i_rem[num_rem].y, i_rem[num_rem].w - move_x * u, i_rem[num_rem].l, &rem);
        i_rem.push_back(rem);
    }
    //удаление выбранного отрезка
    i_rem.erase(i_rem.begin()+num_rem);
    //подготовка к следующей операции окончена НАКАНЕЦТА
    //след итерация, попытки отрезать новые куски
    for(int i = 0; i < i_det.size(); i++)
    {
        for(int j = 0; j < i_rem.size(); j++)
        {
            if (!((i_det[i].length > i_rem[j].l || i_det[i].width > i_rem[j].w) && (i_det[i].length > i_rem[j].w || i_det[i].width > i_rem[j].l)))
            {
                q = q_metric(i_det[i], i_rem[j]);
                guillotine(i, q, j, 0, i_det, i_rem, i_sec, quant_list);
                guillotine(i, q, j, 1, i_det, i_rem, i_sec, quant_list);
            }
        }
        init_rem(0 + w_list*quant_list, 0, w_list, l_list, &rem);
        quant_list++;
        i_rem.push_back(rem);
        int j = i_rem.end() - i_rem.begin() - 1;
        if (!((i_det[i].length > i_rem[j].l || i_det[i].width > i_rem[j].w) && (i_det[i].length > i_rem[j].w || i_det[i].width > i_rem[j].l)))
        {
            q = q_metric(i_det[i], i_rem[j]);
            guillotine(i, q, j, 0, i_det, i_rem, i_sec, quant_list);
            guillotine(i, q, j, 1, i_det, i_rem, i_sec, quant_list);
        }
    }
    //расположили всё
    if (i_det.size() == 0)
    {
        if (res_sections.size() == 0)
        {
            res_sections = i_sec;
            res_squre = res_squr(i_rem);
        }
        else
        {
            if (res_squre > res_squr(i_rem))
            {
                res_sections = i_sec;
                res_squre = res_squr(i_rem);
            }
        }
    }
}

static int res_squr(std::vector<remnant> rem)
{
    int res = 0;
    for(int i = 0; i < rem.size(); i++)
    {
        res += rem[i].squre;
    }
    return res;
}