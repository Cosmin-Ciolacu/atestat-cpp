#include <iostream>
#include <string>
#include <string.h>
#include <windows.h>
#include <mysql.h>
#include <conio.h>
using namespace std;
MYSQL* conn;
int query_state;
const int width = 20;
const int height = 20;
int x, y, fructX, fructY, pct = 0;
int coadaX[100], coadaY[100];
int ncozi;
enum eDirecton { STOP = 0, LEFT, RIGHT, UP, DOWN};
eDirecton dir;
bool ruleaza;
void setup()
{
    ruleaza = true;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    fructX = rand() % width;
    fructY = rand() % height;
}
void deseneaza()
{

    system("cls"); //system("clear");
    for (int i = 0; i < width+2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (j == 0)
                cout << "#";
            if (i == y && j == x)
                cout << "O";
            else if (i == fructY && j == fructX)
                cout << "F";
            else
            {
                bool print = false;
                for (int k = 0; k < ncozi; k++)
                {
                    if (coadaX[k] == j && coadaY[k] == i)
                    {
                        cout << "o";
                        print = true;
                    }
                }
                if (!print)
                    cout << " ";
            }


            if (j == width - 1)
                cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < width+2; i++)
        cout << "#";
    cout << endl;
    cout << "pentru a opri jocul apasa tasta Q" << endl;
    cout << "punctaj optinut" << pct;
}
bool con_to_db()
{
     conn = mysql_init(0);
     conn = mysql_real_connect(conn,"localhost","root","","atestat",0,NULL,0);
     if(conn)
        //cout<<"connection to atestat databse successful "<<endl;
        return true;
     else
       // cout<<"connection problem: "<<mysql_error(conn)<<endl;
       return false;
}

bool exista_nume(string nume)
{
    MYSQL_RES *rez;
    string str_query = "SELECT * FROM punctaje WHERE nume='" + nume + "'";
    const char *query = str_query.c_str();
    int query_state = mysql_query(conn, query);
    if (!query_state)
    {
        rez = mysql_store_result(conn);
       // return mysql_num_rows(rez);
        if(mysql_num_rows(rez) != 0)
            return true;
        else
            return false;

    }

}

void insert_to_db(string nume)
{
    string query = "INSERT INTO punctaje(id, nume, pct) VALUES ('','" + nume + "', '0')";
    const char* q = query.c_str();
   //  cout<<"query is: "<<q<<endl;
   query_state = mysql_query(conn, q);
   if(!query_state)
      cout<<"te-ai inregistrat cu succes";
   else
      cout<<"eroare"<<mysql_error(conn);
}
void input()
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 'a':
            dir = LEFT;
            break;
        case 'd':
            dir = RIGHT;
            break;
        case 'w':
            dir = UP;
            break;
        case 's':
            dir = DOWN;
            break;
        case 'q':
            ruleaza = false;
            break;

        }
    }
     int prevX = coadaX[0];
    int prevY = coadaY[0];
    int prev2X, prev2Y;
    coadaX[0] = x;
    coadaY[0] = y;
    for (int i = 1; i < ncozi; i++)
    {
        prev2X = coadaX[i];
        prev2Y = coadaY[i];
        coadaX[i] = prevX;
        coadaY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    switch (dir)
    {
    case LEFT:
        x--;
        break;
    case RIGHT:
        x++;
        break;
    case UP:
        y--;
        break;
    case DOWN:
        y++;
        break;
    default:
        break;
    }
    //if (x > width || x < 0 || y > height || y < 0)
    //  gameOver = true;
    if (x >= width) x = 0; else if (x < 0) x = width - 1;
    if (y >= height) y = 0; else if (y < 0) y = height - 1;

    for (int i = 0; i < ncozi; i++)
        if (coadaX[i] == x && coadaY[i] == y)
            ruleaza = false;

}
void actualizare_pct(string nume)
{

    // actualizare in db
    MYSQL_RES *rez;
    char pnct[5] = {NULL};
    int i = 0;
    while(pct>0)
    {
        int uc=pct%10;
        pct /= 10;
        pnct[i] = char(48+uc);
        i++;
    }
    int j;
    for(j = 0;j<strlen(pnct);j++)
        swap(pnct[j],pnct[strlen(pnct) - 1]);
    string str_query = "UPDATE punctaje SET pct='" + string(pnct) + "' WHERE nume='" + nume + "'";
    const char *q = str_query.c_str();
    query_state = mysql_query(conn, q);
   if(!query_state)
      cout<<"ai optinut punctajul: "<<string(pnct)<<" punctaj actualizat";
   else
      cout<<"eroare"<<mysql_error(conn);
    cout<<endl;
}
void logic()
{
     if (x == fructX && y == fructY)
    {
        pct += 10;
        fructX = rand() % width;
        fructY = rand() % height;
        ncozi++;
        //actualizare_pct(nume);
    }
}
void afis_clasament()
{
    cout << "clasament";
}
int main()
{
    setup();
    if(con_to_db())
    {
         string nume;
         cout<<"Introdu numele tau ";

         getline(cin, nume);
         if(!exista_nume(nume))
            {
                insert_to_db(nume);
                while(ruleaza)
                {
                    deseneaza();
                    input();
                    logic();
                    Sleep(10);
                }
                if(!ruleaza)
                {
                    actualizare_pct(nume);
                    Sleep(2000);
                    afis_clasament();
                }
            }
         else
            cout<<"numele introdus exista deja"<<endl;
    } else
        cout<<"EROARE DE CONECTARE";
    return 0;
}

