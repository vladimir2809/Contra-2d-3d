#include <SFML/Graphics.hpp>
#include <time.h>
#include <stdio.h>
#include <dos.h>
#include <windows.h>
#include <sstream>
using namespace  sf; 
const double  pi=3.1415926;
const int kolvoPylu=200;// ���������� ����
int const kolvoStens=3;// ���������� ����
int const kolvoBonus=20;// ���������� �������
int const kolvoBabah=20;// ���������� �������
int const timeBabah=35;// ������������ ������
int const map_Size_X=20;
int const map_Size_Y=14; 
int scorePlayer=0,scoreBot=0,starsPlayer=0,starsBot=0;
 sf::RenderWindow window(VideoMode(800, 600), "CONTRA ");
void DrawLine(int x,int y,int x1,int y1,Color color)// ��������� ��������� �����
{
//������� ������ �����, �� ������� ����� ���������� �����:
		sf::VertexArray lines(sf::Lines, 16 /*���������� �����*/);
 
		//����� ��� ������ �� ����� ������ ���� �������:
		lines[0].position = sf::Vector2f(x,y);
		lines[1].position = sf::Vector2f(x1,y1);
		//� �.�.
 
		//����� ��� ������ ����� ��������� ����(��� ����� ��������� ���������):
		lines[0].color = color;
		lines[1].color = color;
		//� �.�.
 
		//� � ����� ������� ��� �� �����:
		window.draw(lines);
	}
double RaschetAngle(int x,int y,int ArrivalX,int ArrivalY)// ��������� ������� ���� �� 2� ������
{
        int fdx,fdy;
       double fxy,ff;
        fdx = x-ArrivalX;        
       fdy = y-ArrivalY;
      fxy= fdx ? (double)fdy/fdx:0;
         if (fdx>0) ff=atan(fxy)*180.0/pi-180;
         else 
        ff=atan(fxy)*180.0/pi;
       if (x==ArrivalX||fxy==0)
        {
           if (y>ArrivalY) ff=-90;           
           if (y<ArrivalY) ff=90;
        }
         ff+=90;
        return ff;
}
 bool ClickMouseLeft(Event event)// ��������� ��������� ������� ������� ����� ������ ����
 {
	 if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
	 //if (event.type == Event::MouseButtonPressed)//���� ������ ������� ����
	//			if (event.key.code == Mouse::Left)//� ������ �����
				{ 
					//std::cout<<"ckick mouse"<<"\n";
					return true;
					
				}
     return false;
 }
 double MovingToAngle(double angle,double angle1)// ������� �������� �������� ���� (����� ��� ���� ��� �� ������ ��� �������) 
{
 int vector=0;
		//if (vector==0)
        {
         if (fabs(angle1-angle)>=180)
         {
          if (angle1>angle) vector=2;
          if (angle1<angle) vector=1;
         }
         else
         {
          if (angle1>angle) vector=1;
          if (angle1<angle) vector=2;
         }
        }
        if (angle>=180) angle-=360;
        if (angle<=-180) angle+=360;
		double speedRotation=1;
        if (vector==1) return angle+ speedRotation;
        // unit[n].f++;
        if (vector==2) return angle- speedRotation;
        //unit[n].f--;
		if  (angle<=angle1+1&&angle>=angle1-1) { vector=0; return angle;};
}
 class Stens// ����� ����
 {
	
public:
	int size;
	 struct Stena// ���� ������
	 {
		 int x,y;
	 };
	 Stena stena[kolvoStens];
	 Stens()// ����������� ����. ������������ �������� �����
	 {
		 size=40;
		 for (int i=0;i<kolvoStens;i++)
		 {
			 int rx= rand()%map_Size_X;
			 int ry= rand()%map_Size_Y-1;
			 stena[i].x=rx*40;
			 stena[i].y=ry*40;
		 }

	 }
	 void DrawStens()// ���������� �����
	 {
		 RectangleShape rectangle(sf::Vector2f(size, size));
		 rectangle.setFillColor(Color(128, 128,128));
		 for (int i=0;i<kolvoStens;i++)
		 {
			 rectangle.setPosition(stena[i].x,stena[i].y);
			 window.draw(rectangle);
		 }
	 }
	 bool InStens (int x,int y,int dx=-1,int dy=-1,bool baruer=false)// ��������� �������� ���������� � �����
	 {
		 for (int i=0;i<kolvoStens;i++)
		 {
			 if (dx==-1&&dy==-1)
			 {
				 //���� �� ������ �������� dx, dy �� ��������� ���� �����
				 if (baruer==false)
				 {
					 
					if (x>=stena[i].x && x<=stena[i].x+size && y>=stena[i].y && y<=stena[i].y+size) return true;

				 }
				 else
				 {
					 // ���� ������ ==������ �� ��������� ��������� �����
					 if (x>=stena[i].x-5 && x<=stena[i].x+size+5 && y>=stena[i].y-5 && y<=stena[i].y+size+5) return true;
				 }
			 }
			 else 
			 {
				 //���� �������� dy, dx  ������� �� ��������� �� ����������� 2 ��������������
				 if (x+dx>=stena[i].x && x<=stena[i].x+size && y+dy>=stena[i].y && y<=stena[i].y+size) return true;
			 }
		 }
		 return false;
	 }
 };
 Stens stens;
 class Marshrut //����� ������� ������ ������� ��� �������� ������
{ 
	int MapPoisk[map_Size_X][map_Size_Y];// ������ ������� ������ �������� ���� ������ ����, ������� ������� ����� ������ �� ������ ������

   public:
       int dlina,dlinaPuti,arrivalX,arrivalY,beginX,beginY;
	   bool being;// ���������� �� �������
       struct PointPuti // ����� ����
       {
			int x,y;
			bool being;
       };
	   PointPuti pointPuti[100/*map_Size_X*map_Size_Y*/];
       Marshrut ()
       {
                dlina=0;
				arrivalX=0;
				arrivalY=0;
       }
void DrawPoisk(Text text)
{
	    RectangleShape rectangle(sf::Vector2f(10,10));
		for (int i=0;i<map_Size_X;i++)
		for (int j=0;j<map_Size_Y;j++)
		{
			rectangle.setPosition(i*40+15,j*40+15);
			if(MapPoisk[i][j]==-1)
			{
				 rectangle.setFillColor(Color(108, 108,255));
				 window.draw(rectangle);
			}
			if(MapPoisk[i][j]==-4)
			{
				 rectangle.setFillColor(Color(255, 108,108));
				 
				 window.draw(rectangle);
			}
			if(MapPoisk[i][j]==-2)
			{
				 rectangle.setFillColor(Color(108, 255,108));
				 
				 window.draw(rectangle);
			}
			//
			//if (MapPoisk[i][j]>0)
			//{
			//	text.setColor(Color(255,255,255));//��������� ����� � �������. ���� ������ ��� ������, �� �� ��������� �� �����
			//	std::ostringstream numRastStr;    // �������� ����������
			//	numRastStr <<MapPoisk[i][j];
			//	text.setString(numRastStr.str());
			//    text.setPosition(i*40+20,j*40+20);
			//	window.draw(text);
			//	 rectangle.setFillColor(Color(108+MapPoisk[i][j]*30, 108,108));
			//	 
			//	 window.draw(rectangle);
			//}
			//
		 
		}
		rectangle.setFillColor(Color(255, 255,108));
		for (int i=0;i<dlina;i++)
		if(pointPuti[i].being==true)
		{
		
			rectangle.setPosition(pointPuti[i].x*40+15,pointPuti[i].y*40+15);
			 window.draw(rectangle);
		}
		for (int i=0;i<map_Size_X;i++) DrawLine(i*40+20,1,i*40+20,600,Color::Red);
		for (int j=0;j<map_Size_Y;j++) DrawLine(1,j*40+20,800,j*40+20,Color::Red);
		
		
		
}
void LoadMapPoisk(int x,int y,int arrX,int arrY,int panzerX=-1,int panzerY=-1)// ���������� ������ � ������ �������������� ��� ������ ����
{
	
	for (int i=0;i<map_Size_X;i++)
	for (int j=0;j<map_Size_Y;j++)
     {
       //������������� ������� ��� ������ ����   
      MapPoisk[i][j]=0;
	  for (int k=0;k<kolvoStens;k++)
	  if (stens.stena[k].x==i*40 && stens.stena[k].y==j*40 ) // ������� � ������ �����
	  {
			  MapPoisk[i][j]=-1; 
			  break;
	  }
      if (x>i*40 &&  x<i*40+40 && y>j*40 && y<j*40+40)
	  {
			  beginX=i;
			  beginY=j;
			  MapPoisk[i][j]=-2;// ������� � ������ ����� ������ ���� 
	  }
	  if (arrX>i*40 &&  arrX<i*40+40 && arrY>j*40 && arrY<j*40+40)
	  {
			  arrivalX=i;
			  arrivalY=j;
			//  MapPoisk[i][j]=-3;// ������� ����� ����� ����
	  }
	  if(panzerX!=-1&&panzerY!=-1)
	  if (panzerX+30>i*40 && panzerX<i*40+40 && panzerY+30>j*40 && panzerY<j*40+40) MapPoisk[i][j]=-4; // ������� ����� ���������� ����������

     }
	for (int i=0;i<100;i++)
	{
		pointPuti[i].being=false;
	}
    arrX=arrX;	   
}

	void  VolnaPuti (int DLINA)// �������������� ����� ���� �� �,�
	{
      dlina=DLINA; 
	  
     for (int k=0;k<dlina;k++) // ���� ��������������� �����
     {
	 for (int i=0;i<map_Size_X;i++)
     for (int j=0;j<map_Size_Y;j++)
     {   	
		 
		
         if ((MapPoisk[i][j]==k&&k!=0)||(MapPoisk[i][j]==-2&&k==0))
         {
			 /*
            if (MapPoisk[i][j-1]==-3||MapPoisk[i+1][j]==-3
				||MapPoisk[i][j+1]==-3||MapPoisk[i-1][j]==-3)
			 {
				dlina=k;
				return 0;
			 }
			*/
           if (i-1>=0)
			   if (MapPoisk[i-1][j]==0) MapPoisk[i-1][j]=k+1;
           if (j+1<map_Size_Y)
            if (MapPoisk[i][j+1]==0) MapPoisk[i][j+1]=k+1;
           if (i+1<map_Size_X)
            if (MapPoisk[i+1][j]==0) MapPoisk[i+1][j]=k+1;
           if (j-1>=0)
             if (MapPoisk[i][j-1]==0) MapPoisk[i][j-1]=k+1;
			
		   if (arrivalY!=0)
			   arrivalY=arrivalY;
          }
           
         }
     
     }
	 dlina=dlina;
	}
	void LoadMarshrut()
	{
		int pointX,pointY;
		dlinaPuti=MapPoisk[arrivalX][arrivalY];	
     pointX=arrivalX; 
	 pointY=arrivalY;
	 pointPuti[dlinaPuti].y=pointY;
	 pointPuti[dlinaPuti].x=pointX;
	 pointPuti[dlinaPuti].being=true;
	 pointPuti[0].y=beginY;
	 pointPuti[0].x=beginX;
	 pointPuti[0].being=true;
	 for (int k=dlinaPuti;k>0;k--)
	 
	 {
		 if (MapPoisk[pointX][pointY-1]==k-1 && pointY>0)
		 {
			 pointY--;
			 pointPuti[k-1].y=pointY;
			 pointPuti[k-1].x=pointX;
			 pointPuti[k-1].being=true;
			 continue;
		 }
		 if (MapPoisk[pointX+1][pointY]==k-1 && pointX<map_Size_X)
		 {
			 pointX++;
			 pointPuti[k-1].y=pointY;
			 pointPuti[k-1].x=pointX;
			 pointPuti[k-1].being=true;
			 continue;
		 }
		 if (MapPoisk[pointX][pointY+1]==k-1 && pointY<map_Size_Y)
		 {
			 pointY++;
			 pointPuti[k-1].y=pointY;
			 pointPuti[k-1].x=pointX;
			 pointPuti[k-1].being=true;
			 continue;
		 }
		 if (MapPoisk[pointX-1][pointY]==k-1 && pointX>0)
		 {
			 pointX--;
			 pointPuti[k-1].y=pointY;
			 pointPuti[k-1].x=pointX;
			 pointPuti[k-1].being=true;
			 continue;
		 }
	 }
	pointX=pointX;
	being=true;// ������� ������ � �������� ��� ������������� ������
	}
};      
 Marshrut marshrut;
 bool Vidimosty(int x,int y,int arrivalX,int arrivalY)// ������� ��������� ����� �����
 {
	 double angle,pointX=x,pointY=y;
	 angle=RaschetAngle( x, y, arrivalX,arrivalY);
	 for (int i=0;i<140;i++)
	 {
		 pointY+=7*sin(pi*(angle-90)/180);
		 pointX+=7*cos(pi*(angle-90)/180);
		 if (stens.InStens(pointX,pointY,-1,-1,true)) return false;
		 if (pointX>arrivalX-10 && pointX<arrivalX+10 
			 && pointY>arrivalY-10 && pointY<arrivalY+10) return true;
	 }
	 return true;
 }
 class Babahs// ����� �������
{
	struct Babah// �����
	{
		int x,y,chet;
		bool being;
		bool SmallBah;
	};
	Babah babah[kolvoBabah];
public:
	void Draw()// ���������� �����
	{
		for (int i=0;i<kolvoBabah;i++)
		if (babah[i].being==true) 
		{
			CircleShape shape(babah[i].chet);
			if (babah[i].chet %2==0) //���� ������ �� �������� ������ ����� ������
			{
				shape.setFillColor(Color(255,0,0));

			}
			else
			{
					shape.setFillColor(Color(255,255,0));
			}
			shape.setPosition(Vector2f(babah[i].x-babah[i].chet,babah[i].y-babah[i].chet));
			window.draw(shape);
		}
	}
	void Registration(int xx,int yy,bool smallBabah=false )// ����������� ������
	{
		for (int i=0;i<kolvoBabah;i++)
		if (babah[i].being==false) 
		{
			babah[i].x=xx;
			babah[i].y=yy;
			babah[i].being=true;
			babah[i].SmallBah=smallBabah;
			break;
		}
	}
	void Server()// ������������ �������
	{
		for (int i=0;i<kolvoBabah;i++)
		if (babah[i].being==true) 
		{
			if ((babah[i].chet<timeBabah && babah[i].SmallBah==false)
				||babah[i].chet<12 && babah[i].SmallBah==true)
			{
				babah[i].chet++;
			}
			else 
			{
				babah[i].being=false;
				babah[i].chet=0;
			}
		}
	}
};
Babahs babahs;
class Bonuses// ����� ������
{
	 public:
	 Image bonusesImage; //������� ������ Image (�����������)
	 Texture bonusesTexture;//������� ������ Texture (��������)
	 Sprite bonusesSprite; 
	 struct Bonus // �����
	 {
	  public:
		  int x,y,tip;
		  bool being;
		  Bonus()
		  {
			  /*
			  x=(rand()%20)*40;
			  y=(rand()%15)*40;
			  being=true;
			  tip= (rand()%3)+1;
			  */
		  }
	 };
	 Bonus bonus[kolvoBonus];
	 Bonuses()
	 {
		
		bonusesImage.loadFromFile("Bonus.png");//��������� � ���� ����
		bonusesImage.createMaskFromColor(Color(255,255,255));
		
		bonusesTexture.loadFromImage(bonusesImage);//�������� � ���� ������ Image (�����������)
		bonusesSprite.setTexture(bonusesTexture);
	 }
	 void Draw()// ���������� ������
	 {
		 for (int i=0;i<kolvoBonus;i++)
		 if(bonus[i].being==true)
		 {
			 if (bonus[i].tip==1) bonusesSprite.setTextureRect(IntRect(1,1,40,40));
			 if (bonus[i].tip==2) bonusesSprite.setTextureRect(IntRect(40,1,40,40));
			 if (bonus[i].tip==3) bonusesSprite.setTextureRect(IntRect(1,40,40,40));
			 bonusesSprite.setPosition(bonus[i].x,bonus[i].y);
			 window.draw(bonusesSprite);
		 }
	 }
	 void NewBonus(int x,int y,int tip,bool rezerv=false)// ����������� ������ ������
	 {
		 int realKolvoBonus=rezerv==false ? kolvoBonus-15 : kolvoBonus;
		 for (int i=0;i<realKolvoBonus;i++)
		 if(bonus[i].being==false)
		 {
			 bonus[i].x=x/40*40;
			 bonus[i].y=y/40*40;
			 bonus[i].tip=tip;
			 bonus[i].being=true;
			
			 break;
		 } 
		 /*
			 for (int i=0;i<realKolvoBonus;i++)
			 for (int j=0;j<realKolvoBonus;j++)
			 if (bonus[i].being==true && bonus[j].being==true && j!=i)
			 if(bonus[i].x==bonus[j].x && bonus[i].y==bonus[j].y)
			 {
				 x=x;
			 }
			 */
	 }
	 int InBonus (int x,int y,int dx=-1,int dy=-1)// ��������� �������� ���������� ����� ��� �������������� � ������
	 {
		 for (int i=0;i<kolvoBonus;i++)
		 if (bonus[i].being==true)
		 {
			 if (dx==-1&&dy==-1)
			 {
					if (x>=bonus[i].x && x<=bonus[i].x+40 && y>=bonus[i].y && y<=bonus[i].y+40) return i;

				
			 }
			 else 
			 {
				 if (x+dx>=bonus[i].x && x<=bonus[i].x+40 && y+dy>=bonus[i].y && y<=bonus[i].y+40) return i;
			 }
		 }
		 return -1;
	 }
	 void KillBonus(int n)// ������� �����
	 {
		 bonus[n].being=false;
	 }
};
Bonuses bonuses;

class Pylu// ����� ����
{
public:
	double x,y,// ����������
		angle, // ����
		dx,dy;// �������� ��� ������
	bool being;
	Pylu ()
	{
		being=false;
	}
	void Draw()// ���������� ����
	{
		if (being==true)
		{
			CircleShape shape(2);
			// ����� ������ ������ ����
			shape.setFillColor(Color(250, 250, 0));
			shape.setPosition(x,y);
			window.draw(shape);
		}
	}
	void Registration(int xx,int yy, double angle1,double dx1=0, double dy1=0)// �������� ����
	{
		if (being==false)
		{
			x=xx;
			y=yy;
			angle=angle1;
			dx=dx1;
			dy=dy1;
			being=true;
		}
	}
	void Server()// ������������ ����
	{
		if (being==true)
		{
			y+=15*sin(pi*(angle-90)/180)+dy;
			x+=15*cos(pi*(angle-90)/180)+dx;
			if (y<0||y>600||x<0||x>800) being=false;
			if (stens.InStens(x,y))
			{
				being=false;
				babahs.Registration(x,y,true);
			}
		}
		
	}

};
Pylu pylu[kolvoPylu];
void PoletPylus()// �������� ��������� ������� ����
{
	for (int i=0;i<kolvoPylu;i++)
	{
		pylu[i].Server();
	}
}
void vustrel(int x,int y, double angle,double dx,double dy)// ����������� ����� ����
{
	for (int i=0;i<kolvoPylu;i++)
	{
		if (pylu[i].being==false)
		{
			pylu[i].Registration(x,y,angle,dx,dy);
			break;
		}
	}
}
class Panzer// ����� ���� ������
{
protected:
	double x,y;
	bool being;
	int chetKill;// ������� ������� ����� ���� ����
	int pos;
	int size;
	int chetAttack,timeAttack;
	int pyluInMagaz;
	int HP;
	int BRONY;
	Color color;
	double angle;
	double turnX,turnY,turnX1,turnY1;// ����� ��������� �����
public:
	Panzer ()
	{
		size=15;
		x=140;
		y=100;
		angle=90;
		chetAttack=0;
		timeAttack=50;
		pyluInMagaz=5;
		HP=100;
		BRONY=100;
		being=true;
		chetKill=0;
		color=Color::Green;
	}
	void Draw()// ���������� ���� 
	{
		if (being==true)
		{
			CircleShape shape(size);
			shape.setFillColor(color);
			shape.setPosition(Vector2f(x,y));
			DrawLine (turnX1,turnY1,turnX,turnY,Color::White);
			window.draw(shape);
		}
	}
	void ChangePosition(int newPos)
	{
	}
	void Move(double dx, double dy) // �������� �����
	{
		x+=dx;
		y+=dy;
	}
	void Turn(double angle)// ������� ����� � ������������ ����
	{
		//int size1=size/3;
		turnY=size*2*sin(pi*(angle-90)/180)+y+size;
		turnX=size*2*cos(pi*(angle-90)/180)+x+size;
		turnY1=size*sin(pi*(angle-90)/180)+y+size; 
		turnX1=size*cos(pi*(angle-90)/180)+x+size;
	}
	virtual void Upravlenie(Event event)//������� ���������� ������ � ����������
	{
		double angleMouse, // ���� �������� ���� ������� ����
			speed=1.5,// �������� �������� �����
			dx=0,dy=0;// �������� �������� �� ��������� � �����������
		int vector=0;
		if (Keyboard::isKeyPressed(Keyboard::W)) // �������� �����
		{
			if (stens.InStens(x,y-speed,size*2,size*2)==false)// ���� �� �������� � �����
			{
				Move(0,-speed);
				dy=-speed;

			}
		}
		if (Keyboard::isKeyPressed(Keyboard::D)) // �������� ������
		{
			if (stens.InStens(x+speed,y,size*2,size*2)==false)
			{
				Move(speed,0);
				dx=speed;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::S)) // �������� ����
		{
			if (stens.InStens(x,y+speed,size*2,size*2)==false)
			{
				Move(0,speed);
				dy=speed;
			}
		}
		if (Keyboard::isKeyPressed(Keyboard::A)) // ������� �����
		{
			if (stens.InStens(x-speed,y,size*2,size*2)==false)
			{
				Move(-speed,0);
				dx-=speed;
			}
		}
		if (x<=0||x+size*2>=800||y<=0||y+size*2>=560)// ���� �������� � ������� �������� ����
		{
			Move(-dx,-dy);
		}
		Vector2i mousePos = Mouse::getPosition(window);//�������� ���������� �������
		
		angleMouse=RaschetAngle(x+size,y+size,mousePos.x,mousePos.y);// ������ ���� ����
		if( ClickMouseLeft(event) && chetAttack>=timeAttack && pyluInMagaz>0)// ������� ��������
		{
				vustrel(turnX,turnY,angle+(rand() % 7)-4,dx,dy);
				pyluInMagaz--;
				chetAttack=0;
		}
		// ������� ����� ���� ���� ��������� ����
		angle=MovingToAngle(angle,angleMouse);	 
		Turn(angle);
	}
	bool InMe (int xx,int yy,int dx1=-1,int dy1=-1)//���������� ����� ��� �������������� � �����
	 {
	
			 if (dx1==-1&&dy1==-1)
			 {
				if (xx>=x && xx<=x+size*2 && yy>=y && yy<=y+size*2) return true;
			 }
			 else 
			 {
				 if (xx+dx1>=x && xx<=x+size && yy+dy1>=y && yy<=y+size) return true;
			 }
		 
		 return false;
	 }

	void Server()// ��������� ��� ������������ �����
	{
		if (being==true)
		{
			if (chetAttack<timeAttack)	chetAttack++;
		}
		else
		{
			if (chetKill<timeBabah)// ���� ���� ����������� 
			{
				chetKill++;
			}
			else
			{
				chetKill=0;
				being=true;
			}

		}
	 
	}
	void KIlled()
	{
		being=false;
	}
	int Get_X()
	{
		return x;
	}
	int Get_Y()
	{
		return y;
	}
	void Put_X(int xx)
	{
		x=xx;
	}
	void Put_Y(int yy)
	{
		y=yy;
	}
	int Get_Size()
	{
		return size;
	}
	int Get_Pylu()
	{
		return pyluInMagaz;
	}

	void Add_Pylu(int value)
	{
		pyluInMagaz+=value;
		if (pyluInMagaz>30) pyluInMagaz=30;
	}
	void Put_Pylu(int value)
	{
		pyluInMagaz=value;
	}
	int Get_HP()
	{
		return HP;
	}

	void Add_HP(int value)
	{
		HP+=value;
		if (HP>100) HP=100;
		if (HP<0) HP=0;
	}
	void Put_HP(int value)
	{
		HP=value;
	}

	int Get_Brony()
	{
		return BRONY;
	}

	void Add_Brony(int value)
	{
		BRONY+=value;
		if (BRONY>100) BRONY=100;
		if (BRONY<0) BRONY=0;
	}
	void Put_Brony(int value)
	{
		BRONY=value;
	}
	int Get_ChetAttack()
	{
		return chetAttack;
	}
	int Get_TimeAttack()
	{
		return timeAttack;
	}
	void Add_TimeAttack(int value)
	{
		timeAttack+=value;
		if (timeAttack>100) timeAttack=100;
		if (timeAttack<3) timeAttack=3;
		chetAttack=timeAttack;
	}
	int Get_Being()
	{
		return being;
	}
};
Panzer panzer;
class PanzerBot : public Panzer// ����� ����� ���� ������������ �� ����� ������
{
	int vector;// ������ �������� �����
	int numStep;// ����� ���������� ������, ����� �� ����
	double speed,dx,dy;// �������� �������� � �������� �� � � �
	bool vidit;// ����� �� ��� ������ 
	bool motionToPuty;// �������� �� ��� �� ����
	bool motion;// ������ �� ��� �� ������ � ������ ����
	bool normalizePuty;// ����������� �� ���� ���� 
public:
	PanzerBot()
	{
		x=330;
		y=320;
		color=Color::Red;
	    //vector=(rand()%4)+1;
		vidit=false;
		chetAttack=0;
		timeAttack=50;
		HP=100;
		BRONY=100;
		speed =1.5;
		motionToPuty=false;

	}
	void ToGoBonus()// ��������� ������� ����, ����� ������ �����
	{
		for (int i=0;i<kolvoBonus;i++)
		if (bonuses.bonus[i].being==true)
		if (Vidimosty(x+size,y+size,bonuses.bonus[i].x+20,bonuses.bonus[i].y+20))
		{
			if (bonuses.bonus[i].x+20>=x+size-2 && bonuses.bonus[i].x+20<=x+size+2)
			{
				if(bonuses.bonus[i].y+20>=y+size) vector=3;
				if(bonuses.bonus[i].y+20<=y+size) vector=1;
			}
			if (bonuses.bonus[i].y+20>=y+size-2 && bonuses.bonus[i].y+20<=y+size+2)
			{
				if(bonuses.bonus[i].x+20>=x+size) vector=2;
				if(bonuses.bonus[i].x+20<=x+size) vector=4;
			}
		}
	}
	void MoveToVector()
	{
		if (vector==1) // �������� �����
		{
			if (stens.InStens(x,y-speed,size*2,size*2)==false)
			{
				Move(0,-speed);
				dy=-speed;

			}else vector=(rand()%4)+1;
		}
		if (vector==2) // �������� ������
		{
			if (stens.InStens(x+speed,y,size*2,size*2)==false)
			{
				Move(speed,0);
				dx=speed;
			}else vector=(rand()%4)+1;
		}
		if (vector==3) // �������� ����
		{
			if (stens.InStens(x,y+speed,size*2,size*2)==false)
			{
				Move(0,speed);
				dy=speed;
			}else vector=(rand()%4)+1;
		}
		if (vector==4)// �������� ������
		{
			if (stens.InStens(x-speed,y,size*2,size*2)==false)
			{
				Move(-speed,0);
				dx-=speed;
			}else vector=(rand()%4)+1;
		}
	}

	void CancelMovingToPuty()// �������� ������� ������ �� ����
	{
			motionToPuty=false;
			numStep=0;
			marshrut.being=false;
	}
	
	bool NormalizeX()
	{
		if(x+size>=marshrut.pointPuti[numStep].x*40+20-speed
			&& x+size<=marshrut.pointPuti[numStep].x*40+20+speed ) return true;
		return false;
	}
	bool NormalizeY()
	{
		if(y+size>=marshrut.pointPuti[numStep].y*40+20-speed
			&& y+size<=marshrut.pointPuti[numStep].y*40+20+speed ) return true;
		return false;
	}
	void BeginNormalize()
	{
		if (NormalizeX()!=true||NormalizeY()!=true)
		{
			normalizePuty=true;
		}
	}
	// ������� ������� ���������� �� ����� ����� ������ ����� ��� ���� ����� ��� �� �������� � �����
	void NormalizeMovingToPuty()
	{
		
		if (NormalizeX()==true||NormalizeY()==true)
		{
			normalizePuty=false;
		}
		
		if (motionToPuty==true && marshrut.being==true  &&
			normalizePuty==true
			)
		{
			if (marshrut.pointPuti[numStep+1].y*40+20<=y+size 
				|| marshrut.pointPuti[numStep+1].y*40+20>=y+size) // ���� ����� ���� ��������� ���� ��� ����
			{
				if (marshrut.pointPuti[numStep+1].x*40+20<x+size)		vector=4;
				if (marshrut.pointPuti[numStep+1].x*40+20>x+size)		vector=2;
				if (NormalizeX()==true) normalizePuty=false;
			}
			else 
			if (marshrut.pointPuti[numStep+1].x*40+20<=x+size 
				|| marshrut.pointPuti[numStep+1].x*40+20>=x+size) // ���� ����� ���� ��������� ����� ��� ������
			{
				if (marshrut.pointPuti[numStep+1].y*40+20<y+size)		vector=1;
				if (marshrut.pointPuti[numStep+1].y*40+20>y+size)		vector=3;
				if (NormalizeY()==true) normalizePuty=false;
			}

		}
		
	}	
	void RegPointAim(int arrivalX,int arrivalY)// ����������� ����� ���� ������ �������� ���
	{
		motionToPuty=true;
		BeginNormalize();
	}
	void MovingToPuty()// ������� �������� ���� �� ����
	{
		if (motionToPuty==true && marshrut.being==true)
		{
		//	vector=1;
			if (x+size>marshrut.pointPuti[numStep].x*40
			&& x+size<marshrut.pointPuti[numStep].x*40+40
			&& marshrut.pointPuti[numStep].y*40+20<y+size)
			{
				vector=1;
			}
			if (y+size>marshrut.pointPuti[numStep].y*40
			&& y+size<marshrut.pointPuti[numStep].y*40+40
			&& marshrut.pointPuti[numStep].x*40+20>x+size)
			{ 
				vector=2;
			}
			if (x+size>marshrut.pointPuti[numStep].x*40
			&&  x+size<marshrut.pointPuti[numStep].x*40+40
			&& marshrut.pointPuti[numStep].y*40+20>y+size)
			{
				vector=3;
			}
			if (y+size>marshrut.pointPuti[numStep].y*40
			&& y+size<marshrut.pointPuti[numStep].y*40+40
			&& marshrut.pointPuti[numStep].x*40+20<x+size) 
			{
				vector=4;
			}
		}
		if(x+size>marshrut.pointPuti[numStep].x*40
			&& x+size<marshrut.pointPuti[numStep].x*40+40
			&& y+size>marshrut.pointPuti[numStep].y*40
			&& y+size<marshrut.pointPuti[numStep].y*40+40)
		{
			numStep++;
		}
		int dlinaPuti=marshrut.dlinaPuti;
		if(x+size>marshrut.pointPuti[dlinaPuti].x*40
			&& x+size<marshrut.pointPuti[dlinaPuti].x*40+40
			&& y+size>marshrut.pointPuti[dlinaPuti].y*40
			&& y+size<marshrut.pointPuti[dlinaPuti].y*40+40)
		{
			CancelMovingToPuty();
		}
	}
	void Upravlenie()// ������� ���������� ����
	{
		double angleAim;
		int vector1=vector;
	    dx=0,dy=0;
		//ToGoBonus();
		MovingToPuty();
		//NormalizeMovingToPuty();
		MoveToVector();
		if (x<=0||x+size*2>=800||y<=0||y+size*2>=560)// ���� �������� � ������� ���� ����
		{
			do// ������ ������ ���� �� ����� ����������� �������
			{
				vector=(rand()%4)+1;
			}while(vector1==vector);
			vector1=vector;
			Move(-dx,-dy);
		}
		//if (rand()%200==1) vector=(rand()%4)+1;// ������� ������ �������� � ������������ 1 �� 200
		
		if (panzer.Get_Being()==true && 0!=0)// ������� ����� ����
		if (pyluInMagaz>0)// ���� ���� ������ 0
		if (Vidimosty(x+size,y+size,panzer.Get_X()+panzer.Get_Size(),
			panzer.Get_Y()+panzer.Get_Size()))// ���� ����� ���� ������
		{
			// ������ ���� ��� �����
			angleAim=RaschetAngle(x+size,y+size,panzer.Get_X()+panzer.Get_Size(),panzer.Get_Y()+panzer.Get_Size());
			vector=0;// ��������� �������� �����
			vidit=true;
			angle=MovingToAngle(angle,angleAim);// �������� ����� ����� ����� ����
			//angle=angleAim;
			if(angle>=angleAim-1 && angle<=angleAim+1 && chetAttack>=timeAttack && pyluInMagaz>0)// ������� �������� ����
 			{
 					vustrel(turnX,turnY,angle+(rand() % 7)-4,dx,dy);
					pyluInMagaz--;
	 				chetAttack=0;
			}
		}
		else 
		{
			if(vidit==true)// ���� ����� ���� � ���� ���������
			{
				vector=(rand()%4)+1;
				vidit=false;
			}

		}
		if (vector==0 && vidit==true && pyluInMagaz<=0)// ���� ��� ����� ������ �� � ���� ����������� �������
			{
				vector=(rand()%4)+1;
				vidit=false;
			}
		Turn(angle);// �������� ����� ���� � ����������� ����� ����
	}
};
PanzerBot panzerBot;
void NewBonusPyle()// ���������� ������� ���� � �������� 
{
	static int chet=0;
	int x,y,kolvoBonusPylu=0;
	if (chet>170)
	{
		chet=0;
		do
		{
			x=(rand()% 740)+30;
			y=(rand()%530)+30;
		}
	    while (stens.InStens(x,y)==true|| bonuses.InBonus(x,y)!=-1
			||	panzer.InMe(x-40,y-40,80,80)==true||panzerBot.InMe(x-40,y-40,80,80)==true);
		//bonuses.NewBonus(x,y,(rand()%2)+2);
		bonuses.NewBonus(x,y,1);
	}
	// ���� 5 ������� ���� ���������� �� �� ������� ����� ��������� ������ ������
	for (int i=0 ;i<kolvoBonus;i++)
	{

		if (bonuses.bonus[i].being==true)
		if (bonuses.bonus[i].tip==1) kolvoBonusPylu++;
	}
	if (kolvoBonusPylu!=5) chet++; else chet=0;
}

class GameInterface// ����� ������ ������� ���������� ����� �����
{
	int HP,BRONY,PYLU,TimeAttack,ChetAttack;
public:
	void LoadData(int hp,int brony,int pylu1,int chetattack,int timeattack)// ������� ������
	{
		HP=hp;
		BRONY=brony;
		PYLU=pylu1;
		TimeAttack=timeattack;
		ChetAttack=chetattack;

	}
	void DrawPylu()// ���������� ���������� ����
	{
		for (int i=1;i<=PYLU;i++)
		{
			DrawLine(299+i*3,590,299+i*3,575,Color(255,255,0) );
		}

	}
	void DrawTimeAttack()// ���������� ����� �����������
	{
		RectangleShape rectangle(sf::Vector2f((int)((float)ChetAttack / (float)TimeAttack*90),5 ));
		rectangle.setPosition(302,569);
		rectangle.setFillColor(Color(128, 255,128));
		window.draw(rectangle);
	}
	void DrawHP()// ���������� ������ ��������
	{
		RectangleShape rectangle(sf::Vector2f((int)((float)HP / (float)100*90),10 ));
		rectangle.setPosition(10,579);
		rectangle.setFillColor(Color(255, 125,128));
		window.draw(rectangle);
	}
	void DrawBRONY()// ���������� ������ �����
	{
		RectangleShape rectangle(sf::Vector2f((int)((float)BRONY / (float)100*90),10 ));
		rectangle.setPosition(10,569);
		rectangle.setFillColor(Color(128, 125,255));
		window.draw(rectangle);
	}
	class Statistika
	{
		Image starsImage;
		Texture starsTexture;//������� ������ Texture (��������)
		Sprite starsSprite;
	public:
		Statistika()
		{
			starsImage.loadFromFile("Stars.png");//��������� � ���� ����
			starsImage.createMaskFromColor(Color(255,255,255));
		
			starsTexture.loadFromImage(starsImage);//�������� � ���� ������ Image (�����������)
			starsSprite.setTexture(starsTexture);
		}
		void DrawStarsPlayer(Text text)
		{
			if (starsPlayer<16)text.setColor(Color(255,255,0)); else text.setColor(Color(255,0,0));
			std::ostringstream starsString;    // �������� ����������
			starsString <<starsPlayer;

			text.setString(starsString.str());
			text.setPosition(450,577);
			starsSprite.setTextureRect(IntRect(1,1,40,40));
			starsSprite.setPosition(470,563);
			window.draw(starsSprite);
			window.draw(text);
		}
		void DrawStarsBot(Text text)
		{
			if (starsBot<16)text.setColor(Color(255,255,0)); else text.setColor(Color(255,0,0));
			std::ostringstream starsString;    // �������� ����������
			starsString <<starsBot;

			text.setString(starsString.str());
			text.setPosition(663,577);
			starsSprite.setTextureRect(IntRect(40,1,40,40));
			starsSprite.setPosition(625,563);
			window.draw(starsSprite);
			window.draw(text);
		}
		void DrawTablo(Text text)
		{
			text.setColor(Color(255,255,255));//��������� ����� � �������. ���� ������ ��� ������, �� �� ��������� �� �����
			std::ostringstream scoreString[2];    // �������� ����������
			scoreString[0] <<scorePlayer;
			scoreString[1] <<scoreBot;
			text.setString(scoreString[0].str()+" : "+scoreString[1].str());
			if (scorePlayer<10) text.setPosition(550,565); else text.setPosition(542,565);
			window.draw(text);
		//	text.setStyle(sf::Text::Bold | sf::Text::Underlined);//������ � ������������ �����. �� ��������� �� "�����":)) � �� ������������
		    
		}
		void Draw(Text text)
		{
			 DrawTablo(text);
			 DrawStarsPlayer(text);
			 DrawStarsBot(text);
		}
	};
	Statistika statistika;
	
	void Draw()// ���������� ���������
	{
		DrawLine(1,560,800,560,Color(128,128,128));
		DrawPylu();
		DrawTimeAttack();
		DrawHP();
		DrawBRONY();
	}
	
};
GameInterface gameInterface;
void BodborBonus()// ������� ������� �������
{
 int valuePanzer=-1,// ����� ������������ ������ �������
	 valuePanzerBot=-1;// ����� ������������ ������ �����
 valuePanzer=bonuses.InBonus(panzer.Get_X(),panzer.Get_Y(),panzer.Get_Size()*2,panzer.Get_Size()*2);
 valuePanzerBot=bonuses.InBonus(panzerBot.Get_X(),panzerBot.Get_Y(),
							panzerBot.Get_Size()*2,panzerBot.Get_Size()*2);
 if (valuePanzer!=-1) 
 {
		 bonuses.KillBonus(valuePanzer);
		 if (bonuses.bonus[valuePanzer].tip==1) panzer.Add_Pylu(5);// 1- ����� ����
		 if (bonuses.bonus[valuePanzer].tip==2)// 2- ����� ���������
		 {
			 panzer.Add_TimeAttack(-3);
			 starsPlayer++;
		 }
		 if (bonuses.bonus[valuePanzer].tip==3) panzer.Add_Brony(100);// 3-����� �����
 }
 if (valuePanzerBot!=-1) 
 {
		 bonuses.KillBonus(valuePanzerBot);
		 if (bonuses.bonus[valuePanzerBot].tip==1) panzerBot.Add_Pylu(5);
		 if (bonuses.bonus[valuePanzerBot].tip==2) 
		 {
			 panzerBot.Add_TimeAttack(-3);
			 starsBot++;
		 }
		 if (bonuses.bonus[valuePanzerBot].tip==3) panzerBot.Add_Brony(100);
 }
}
void NewCoordinateKill()// ����� ���������� ����� ������ �����
	{
		bool killPanzer=false,killPanzerBot=false;
	  for (int i=0;i<kolvoPylu;i++)
	  {
	  if (pylu[i].being==true)
	  {
			if (panzer.InMe(pylu[i].x,pylu[i].y))// ���� � ������ ������ ����
			{
				//if (panzer.Get_Brony()>=25)
				//������ �������� � �����
				if(panzer.Get_Brony()>0) 
				{
					panzer.Add_Brony(-25);	
					if (panzer.Get_Brony()==0)
					{
					 pylu[i].being=false;
					  break;
					}
				}
				if (panzer.Get_Brony()<=0) 
				{
					panzer.Add_HP(-25);
				
				}
			
			
				if (panzer.Get_HP()<=0)	// ���� �������� ���
				{
						killPanzer=true;
						panzer.KIlled();
						// ����������� ������
						babahs.Registration(panzer.Get_X()+panzer.Get_Size(),
							                 panzer.Get_Y()+panzer.Get_Size());
						// ����������� ������
						bonuses.NewBonus(panzer.Get_X()+panzer.Get_Size(),
							panzer.Get_Y()+panzer.Get_Size(),(rand()%2)+2,true);
						scoreBot++;
				}
				else
				{
					babahs.Registration(pylu[i].x,pylu[i].y,true);
				}
				pylu[i].being=false;
				break;
			}
			// ���������� ��� ��� ��� ����� ������
			if (panzerBot.InMe(pylu[i].x,pylu[i].y))
			{
				if(panzerBot.Get_Brony()>0) 
				{
					panzerBot.Add_Brony(-25);	
					if (panzerBot.Get_Brony()==0)
					{
						pylu[i].being=false;
						break;
					}
				}
				if (panzerBot.Get_Brony()<=0) 
				{
					panzerBot.Add_HP(-25);
					
				}
				
				if (panzerBot.Get_HP()<=0)	
				{
						killPanzerBot=true;
						panzerBot.KIlled();
						babahs.Registration(panzerBot.Get_X()+panzerBot.Get_Size(),
							                 panzerBot.Get_Y()+panzerBot.Get_Size());
						bonuses.NewBonus(panzerBot.Get_X()+panzerBot.Get_Size(),
										panzerBot.Get_Y()+panzerBot.Get_Size(),(rand()%2)+2,true);
						scorePlayer++;
				}
				else
				{
					babahs.Registration(pylu[i].x,pylu[i].y,true);
				}
				pylu[i].being=false;
				break;
			}
	  }
	  }
	  int x,y;
	  if (Keyboard::isKeyPressed(Keyboard::Q)) 
	  {
		  killPanzer=true;
		  Sleep (10);
		  //for (int i=0;i<10000000;i++);
	  }
	    if (Keyboard::isKeyPressed(Keyboard::R)) 
	  {
		  killPanzerBot=true;
		  Sleep (10);
	  }
	  bool noVidit=false;
	  bool noStens=false;
	  if (killPanzer || killPanzerBot)// ���� ���� ����� ��� ��� �� ��������� ������ ����������
	  {
		do
		{
			noVidit=false;
	        noStens=false;
			x=(rand()% 740)+30;
			y=(rand()%500)+30;
			// ���� � � � �� �������� � ����� 
			if (stens.InStens(x,y,panzer.Get_Size()*2,panzer.Get_Size()*2)==false) noStens=true;
			
			{
				//���� � � � �� �������� ��� ���� �������� �����
				if (killPanzer)
				{
					if (Vidimosty (x+panzer.Get_Size(),y+panzer.Get_Size(),
					panzerBot.Get_X()+panzerBot.Get_Size(),
					panzerBot.Get_Y()+panzerBot.Get_Size() )==false )
					{
						noVidit=true;
					
					}
				}
				if (killPanzerBot)
				{
					if (Vidimosty (x+panzerBot.Get_Size(),y+panzerBot.Get_Size(),
					panzer.Get_X()+panzer .Get_Size(),
					panzer.Get_Y()+panzer.Get_Size() )==false )
					{
						noVidit=true;
					
					}
				}
			}
	// ���� � � � �� �������� � ����� , �� ��������� � ���� ��������� �����, � �� �������� �� �����
		}while ((noStens==false || noVidit==false)
				||bonuses.InBonus(x-panzer.Get_Size(),y-panzer.Get_Size(),
				                  panzer.Get_Size()*3,panzer.Get_Size()*3)!=-1
				||bonuses.InBonus(x-panzerBot.Get_Size(),y-panzerBot.Get_Size(),
				                  panzerBot.Get_Size()*3,panzerBot.Get_Size()*3)!=-1);
		if (killPanzer)// ���� ���� ������ ��� ���� 
		{
			// �� ��������� ��� ����� ��������� � �������� ���������
			killPanzer=false;
			panzer.Put_Pylu(5);
			panzer.Put_HP(100);
			panzer.Put_X(x);
			panzer.Put_Y(y);
		}
		// ����������
		if (killPanzerBot)
		{
			killPanzerBot=false;
			panzerBot.Put_Pylu(5);
			panzerBot.Put_HP(100);
			panzerBot.Put_X(x);
			panzerBot.Put_Y(y);
		}
	  }
	  /*
	    if (killPanzerBot)
	  {
		do
		{
			noVidit=false;
	        noStens=false;
			x=(rand()% 740)+30;
			y=(rand()%540)+30;
			if (stens.InStens(x,y,panzerBot.Get_Size()*2,panzerBot.Get_Size()*2)==false) noStens=true;
			
			{
				if (Vidimosty (x+panzerBot.Get_Size(),y+panzerBot.Get_Size(),
				panzer.Get_X()+panzer.Get_Size(),panzer.Get_Y()+panzer.Get_Size() )==false )
				{
					noVidit=true;
					
				}
			}
		}while (!(noStens==true && noVidit==true));
		killPanzer=false;
		panzer.Put_X(x);
		panzer.Put_Y(y);
	  }
		*/
	}
int main()// ������� ������� 
{
	
	double time1=0,time2=0; // ��������� ��� �������
    srand(time(0));
	Clock clock;
	Font font;//����� 
	font.loadFromFile("times-new-roman.ttf");//�������� ������ ������ ���� ������
	Text text("", font, 20);//������� ������ �����. ���������� � ������ ����� ������, �����, ������ ������(� ��������);//��� ������ ����� (�� ������)
			
	while (window.isOpen())
	{
		clock.restart();
		time1=clock.getElapsedTime().asMicroseconds();// �������� ����� � ������ �����
		Event event;
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
				window.close();
		}
		panzer.Upravlenie(event);
		panzerBot.Upravlenie();
		panzer.Server();
		panzerBot.Server();
		babahs.Server();
		PoletPylus();
		/*
		if (Keyboard::isKeyPressed(Keyboard::R)) 
	    {
			bonuses.NewBonus(rand()%800,rand()%541,1);
		  Sleep (10);
	    }
		if (Keyboard::isKeyPressed(Keyboard::Z)) 
	    {
			bonuses.NewBonus(rand()%800,rand()%541,1);
		  Sleep (10);
	    }
		*/
		if (Keyboard::isKeyPressed(Keyboard::M)) 
	    {
			Vector2i mousePos = Mouse::getPosition(window);//�������� ���������� �������
			marshrut.LoadMapPoisk(panzerBot.Get_X()+15,panzerBot.Get_Y()+15,
				mousePos.x,mousePos.y );
			marshrut.VolnaPuti(25);
			marshrut.LoadMarshrut();
			panzerBot.RegPointAim(1,1);
		  Sleep (10);
	    }
		NewBonusPyle();
		BodborBonus();                                        //  Get_ChetAttack()
		gameInterface.LoadData(panzer.Get_HP(),panzer.Get_Brony(),
			       panzer.Get_Pylu(),panzer.Get_ChetAttack(),panzer.Get_TimeAttack());
		NewCoordinateKill();
		window.clear();
		panzer.Draw();
		panzerBot.Draw();
		for (int i=0;i<kolvoPylu;i++)	pylu[i].Draw();
		stens.DrawStens();

		bonuses.Draw();
		babahs.Draw();
		gameInterface.Draw();
		gameInterface.statistika.Draw(text);
		marshrut.DrawPoisk(text);
		window.display();
		
		do 
		{
			time2=clock.getElapsedTime().asMicroseconds();// �������� ����� � ����� �����
			//Sleep (1);
		}
		while (time2-time1<15000);// ���� ������� �� ������� ����� ������� � ������ ����� 
		//������ ������������� ����� �� ��������� ����� ������ 
	}
 
	return 0;
}
