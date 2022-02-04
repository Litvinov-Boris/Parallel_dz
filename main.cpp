#include "guillotine.h"
#include "mpi.h"
#include <time.h> 

#define FIRST_THREAD 0

int main(int argc, char **argv)
{
    int thread, thread_size, processor_name_length;
    char* processor_name = new char[MPI_MAX_PROCESSOR_NAME * sizeof(char)];
    MPI_Status status;
    // Инициализируем работу MPI
	MPI_Init(&argc, &argv);
	// Получаем имя физического процессора
	MPI_Get_processor_name(processor_name, &processor_name_length);
	// Получаем номер конкретного процесса на котором запущена программа
	MPI_Comm_rank(MPI_COMM_WORLD, &thread);
	// Получаем количество запущенных процессов
	MPI_Comm_size(MPI_COMM_WORLD, &thread_size);
    std::vector<detail> details;
    int w_map, l_map, col_det;
    clock_t start, end;
    if (read_order(&w_map, &l_map, &details) == 0)
    {
        printf("ошибка считывания\n");
        MPI_Finalize();
        return 0;
    }
    if (thread == FIRST_THREAD){
        start = clock();}
    Order order(w_map, l_map, details);
    int diap_start, diap_end, shag, od;
    if (order.details.size() * 2 <= thread_size)
    {
        diap_start = thread / 2;
        diap_end = diap_start + 1 <= order.details.size() ? diap_start + 1 : order.details.size();
        order.chek_variants(diap_start, diap_end, (thread_size % 2));
    }
    else{
        od = order.details.size() % thread_size;
        shag = thread < od ? (order.details.size() / thread_size) + 1 : order.details.size() / thread_size;
        if (thread < od)
            diap_start = thread * shag;
        else if (thread == od)
            diap_start = thread * (shag + 1);
        else
            diap_start = (od * (shag + 1)) + (shag * (thread - od));
        diap_end = diap_start + shag;
        order.chek_variants(diap_start, diap_end, 0);
        order.chek_variants(diap_start, diap_end, 1);
    }
    
    int *res;
    res = (int*)malloc(sizeof(int) * (order.res_sections.size() * 3 + 1));
    res[0] = order.res_squre;
    int i = 0;
    int j = 1;
    while (i < order.res_sections.size())
    {
        res[j] = order.res_sections[i].x;
        j++;
        res[j] = order.res_sections[i].y;
        j++;
        res[j] = order.res_sections[i].diretion;
        j++;
        i++;
    }
    if (thread != FIRST_THREAD)
    {
        MPI_Send (res, (int)(order.res_sections.size() * 3 + 1),MPI_INT, 0, 0, MPI_COMM_WORLD);
    }
    else
    {
        int count;
        int i = 1, t;
        int *j;
        int count_for_print = order.res_sections.size() * 3 + 1;
        while (i < thread_size)
        {
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &count);
            j = (int*)malloc(sizeof(int) * count);
            MPI_Recv(j, count, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (count > 1 && res[0] > j[0])
            {
                free(res);
                res = j;
                count_for_print = count;
            }
            else
                free(j);
            i++;
        }
        end = clock();
        double seconds = (double)(end - start) / CLOCKS_PER_SEC;
        printf("Time %lf\n", seconds);
        print_result(res, count_for_print);
    }
    MPI_Finalize();
    return 0;
}