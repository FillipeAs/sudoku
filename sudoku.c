#include<stdio.h>
#include<stdlib.h>
#include<assert.h>

#define ELEMENT_PER_LINE (9) 
#define ELEMENT_NUM ((ELEMENT_PER_LINE)*(ELEMENT_PER_LINE))
#define BLOCK_WIDTH (3)
#define MAX_PEER_NUM (20)
#define MAX_UNIT_MEMBER (9)

typedef struct  sudoku_elm{
    unsigned int peer[MAX_PEER_NUM] ;
    unsigned int peer_top ;
    unsigned int unit[3][MAX_UNIT_MEMBER];
    unsigned int top_x;
    unsigned int top_y;
    unsigned int top_b;
    unsigned short possibility[9];
}sudoku_elm;

inline init_possibility(struct sudoku_elm* result,int index)
{
    int k = 0;
    for(k  = 0 ; k < 9;k++)
    {
        result[index].possibility[k]=1;
    }
}

inline count_possibility(struct sudoku_elm* result ,int index)
{
    int k = 0;
    int count = 0;
    for(k = 0 ;k < 9; k++)
    {
        if(result[index].possibility[k] == 1)
            count++;
    }
    return count;
}
int get_nth_possibility(struct sudoku_elm* result,int index,int n)
{
    int k = 0; 
    int j = -1;
    for(k = 0 ; k<9;k++)
    {
        if(result[index].possibility[k] == 1)
        {

            j++;
            if(j == n)
                break;
        }
    }
    return k+1;
}
int get_first_possibility(struct sudoku_elm* result ,int index)
{
    return get_nth_possibility(result,index,0);
}

int is_have_possibility(struct sudoku_elm* result,int index,int value)
{
    return (result[index].possibility[value-1] == 1);
}

int clear_possibility(struct sudoku_elm* result,int index,int value)
{
    result[index].possibility[value-1] = 0;
}

init_sudoku(struct sudoku_elm* result)
{
    assert(result != NULL);
    int i=0;
    int j=0;
    memset(result,0,sizeof(struct sudoku_elm)*ELEMENT_NUM);
    for(i = 0 ; i<ELEMENT_NUM;i++)
    {
        init_possibility(result,i);  
        int x = i/(ELEMENT_PER_LINE);
        int y = i%ELEMENT_PER_LINE;
        for(j = 0 ; j<ELEMENT_NUM;j++)
        {
            int cur_x= j/ELEMENT_PER_LINE;
            int cur_y= j%ELEMENT_PER_LINE;

            if(cur_x == x)
                result[i].unit[0][result[i].top_x++] = j;
            if(cur_y == y)
                result[i].unit[1][result[i].top_y++] = j;
            if(cur_x/3 == x/3 &&
               cur_y/3 == y/3)
                result[i].unit[2][result[i].top_b++] = j;
        }
        int k = 0;
        int m = 0;
        for(m = 0; m<2;m++)
        {
            for(k = 0;k<ELEMENT_PER_LINE;k++)
            {
                if(result[i].unit[m][k] != i)
                    result[i].peer[result[i].peer_top++] = result[i].unit[m][k];
            }
        }

        for(m = 0 ; m < BLOCK_WIDTH*BLOCK_WIDTH;m++)
        {
            if(result[i].unit[2][m]/ELEMENT_PER_LINE !=  x &&
                    result[i].unit[2][m]%ELEMENT_PER_LINE != y)
                result[i].peer[result[i].peer_top++] = result[i].unit[2][m];
        }
    }

    for(i = 0 ; i <ELEMENT_NUM; i++ )
    {
        assert(result[i].top_x == 9 &&
                result[i].top_y == 9 &&
                result[i].top_b == 9 &&
                result[i].peer_top == 20);

    }
    return 0;
}

int eliminate(struct sudoku_elm* result,int index,int value)
{
    if(is_have_possibility(result,index,value)== 0 )
    {
        return 0;
    }

    clear_possibility(result,index,value);
    if(count_possibility(result,index) == 0)
    {
        return -1;//contradict;
    }

    int i;
    int ret = 0;
    if(count_possibility(result,index) == 1)
    {
        int data;
        data = get_first_possibility(result,index);
        for(i = 0 ; i < MAX_PEER_NUM; i++)
        {
            int peer = result[index].peer[i];
            ret = eliminate(result,peer,data);
            if(ret != 0)
            {
                return -2;
            }
        }
    }


    int cur_unit = 0;
    int d_place[MAX_UNIT_MEMBER];
    for(cur_unit = 0; cur_unit < 3;cur_unit++)
    {

        memset(d_place,0,sizeof(int)*MAX_UNIT_MEMBER);
        int j = 0;
        for(i = 0 ; i<MAX_UNIT_MEMBER;i++)
        {
            int unit = result[index].unit[cur_unit][i];
            if (is_have_possibility(result,unit,value))
            {
                d_place[j++] = unit;
            }
        }

        if(j == 0)
        {
            return -3;
        }
        else if(j == 1)
        {
            ret = assign(result,d_place[0],value);
            if(ret)
                return -4;
        }
    }

    return 0;
}

int assign(struct sudoku_elm* result ,int index,int value)
{
    if(!(value >= 1 && value <= 9 ))
    {
        return -1;
    }

    int i = 0;
    int failed = 0;
    for(i = 1 ; i <= 9 ; i++)
    {
        if(i == value)
            continue;

        int ret;
        ret = eliminate(result,index,i);
        if(ret)
        {
            failed = 1;
            break;
        }
    }

    if(failed)
    {
        return -2;
    }
    return 0;

}

int parse_input(sudoku_elm* result,char* buf)
{
    int i ; 
    int d;
    int ret;
    int failed = 0;
    for(i = 0 ; i< ELEMENT_NUM; i++)
    {
        if(buf[i] >'0' && buf[i] <='9')
        {
            d = buf[i] - '0';
            ret = assign(result,i,d); 
            if(ret < 0 )
            {
                failed = 1;
                break;
            }
        }
    }
    if(failed)
    {
        return -1;
    }
    else
    {
        return 0;
    }
}

int is_solved(struct sudoku_elm* result)
{
    int i;
    for(i = 0; i < ELEMENT_NUM;i++)
    {
        if(count_possibility(result,i) != 1)
        {
            return 0;
        }
    }
    return 1;
}

int search(struct sudoku_elm* result)
{
    if(is_solved(result) == 1)
        return 0;

    int i = 0; 
    int idx = 0;
    int min_count=10;
    for(i = 0 ; i<ELEMENT_NUM;i++)
    {
        int count = count_possibility(result,i);
        if(count < min_count && count > 1)
        {
            min_count = count;
            idx = i;
        }
    }

    struct sudoku_elm* result_cpy = NULL;
    int success = 0;
    for(i = 0 ; i<min_count;i++)
    {

        result_cpy = malloc(sizeof(struct sudoku_elm)*ELEMENT_NUM);
        if(result_cpy == NULL)
        {
            fprintf(stderr,"can not malloc for try\n");
            return -1;
        }
        memcpy(result_cpy,result,sizeof(sudoku_elm)*ELEMENT_NUM);
        int value = get_nth_possibility(result_cpy,idx,i);
        int ret = assign(result_cpy,idx,value);
        if(ret == 0)
        {
            ret = search(result_cpy);
            if(ret == 0)
            {
                success = 1;
                break;
            }
            else
            {
                free(result_cpy);
                result_cpy = NULL;
            }
        }
        else
        {
            free(result_cpy);
            result_cpy = NULL;
        }
    }
    if(success == 1)
    {
        memcpy(result,result_cpy,sizeof(sudoku_elm)*ELEMENT_NUM);
        free(result_cpy);
        return 0;
    }
    else
    {
        return -1;
    }
}
int display(struct sudoku_elm* result)
{
    int i ;
    int max_count = 0;
    int count; 
    for(i = 0 ; i < ELEMENT_NUM;i++)
    {
        count = count_possibility(result,i);
        if(count > max_count)
            max_count = count;
    }
    int width = max_count + 1;
    int j;
    fprintf(stdout,"------------------------------------------------------------------------------------------\n");
    for (i = 0 ; i < ELEMENT_PER_LINE;i++)
    {
        for(j = 0 ; j< ELEMENT_PER_LINE;j++)
        {
            int k = count_possibility(result,i*9+j);
            int m=0;
            for(m = 0; m <k ;m++)
            {
                fprintf(stdout,"%d",get_nth_possibility(result,i*9+j,m));
            }
            for(;m<width;m++)
                fprintf(stdout," ");
        }
        fprintf(stdout,"\n");
    }
}

int sudoku(char* buf)
{
    int ret = 0;
    struct sudoku_elm* result = malloc(ELEMENT_PER_LINE*ELEMENT_PER_LINE*sizeof(struct sudoku_elm));
    if(result == NULL)
    {
        fprintf(stderr,"malloc for sudoku failed\n");
        goto out;
    }
    init_sudoku(result);

    ret= parse_input(result,buf);
    if(ret)
    {
        fprintf(stderr,"parse failed\n");
        goto out; 
    }
    display(result);
    if( is_solved(result)== 1)
    {
        display(result);
    }
    else
    {
        ret = search(result);
        if(ret != 0 )
        {
            fprintf(stderr,"search failed\n");
            goto out;
        }
        else
        {
            display(result);
        }
    }

out:
    if(result)
        free(result);
    return ret;

}
int main()
{
    
    char buf[128]=".....6....59.....82....8....45........3........6..3.54...325..6..................";
    int ret = sudoku(buf);
    if(ret != 0)
    {
       fprintf(stderr,"failed\n");
       return ret;
    }
    return 0;
}

