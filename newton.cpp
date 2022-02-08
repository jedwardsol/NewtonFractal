#include <Windows.h>
#include <vector>
#include <array>
#include <string>
#include <sstream>
#include <stack>
#include <limits>

#include "bitmap.h"
#include "window.h"


#include <thread>
#include <complex>
#include "newton.h"

constexpr int                           numThreads{6};
std::array<std::thread,numThreads+1>    theThreads;


struct Box
{
    Point                               topLeft;
    Point                               bottomRight;
};

std::stack<Box> history;


Point  fromPixel(int row, int column)
{
    auto &current=history.top();

    return {  current.topLeft.real()  + column  * (current.bottomRight.real() -current.topLeft.real() ) / dim,
              current.topLeft.imag()  + row     * (current.bottomRight.imag() -current.topLeft.imag() ) / dim };
}



auto nearestRoot(Point const &p)
{
    static const Point roots[3] =
    {
	    { 1.0,  0}, 
	    {-0.5,  std::sqrt(3.0)/2.0}, 
	    {-0.5, -std::sqrt(3.0)/2.0}
    };

    struct 
    {
        int     root;
        double  distance;
    } result{0,  std::numeric_limits<double>::infinity() } ;


    for(int i=0;i<3;i++)
    {
        auto v = p-roots[i];

        double distance{std::numeric_limits<double>::infinity()};

        distance = static_cast<double>(abs(p-roots[i]));

        if(distance < result.distance)
        {
            result.root     = i;
            result.distance = distance;
        }
    }

    return result;
}



Point f(Point const &p)
{
    static Point const one{1.0,0.0};

    return pow(p,3) - one;
}

Point df(Point const &p)
{
    static Point const three{3.0,0.0};

    auto slope = three * p * p;

    return slope;
}



// 0-80    root 1
// 80-160  root 2
// 160-240 root 4
// exact value is related to iteration count to converge


int findRoot(Point const &point)
{
    Point       x{point};

    int         count{};

    do
    {
        try
        {
            auto [root, distance] = nearestRoot(x);

            if(distance < 0.1)
            {
                return (root * 80) + (count % 80);
            }

            x = x - f(x)/df(x);
        }
        catch(...)
        {
            std::cout << "No root at " << point << "\n";    // {0.0,0.0}

            return 255;
        }



        count++;
    } while(count < 100);

    return 255;
}

void NewtonLowRes()
{
    constexpr int lowResolution{10};
    static_assert((dim % lowResolution) == 0);

    for(int row=lowResolution;row<dim;row+=lowResolution)
    {
        for(int column=0;column<dim;column+=lowResolution)
        {
            auto c = fromPixel(row, column);

            bitmapData[row][column]   = findRoot(c);

            for(int i=0;i<lowResolution;i++)
            {
                for(int j=0;j<lowResolution;j++)
                {
                    bitmapData[row+j][column+i] = bitmapData[row][column];
                }
            }

            if(done)
            {
                return;
            }
        }
    }
}

void Newton(int startRow)
{
    for(int row=startRow;row<dim;row+=numThreads)
    {
        for(int column=0;column<dim;column++)
        {
            auto c = fromPixel(row, column);

            bitmapData[row][column] = findRoot(c);

            if(done)
            {
                return;
            }
        }

        redrawWindow();
    }
}




void go()
{
    using std::abs;

    auto &current=history.top();

    auto width  = abs(current.bottomRight.real() - current.topLeft.real());
    auto height = abs(current.bottomRight.imag() - current.topLeft.imag());

    auto stringise = [](auto const &val)
    {
        std::ostringstream  s;

        s<<val;

        return s.str();
    };


    auto title = std::format("Newton width {}  height {}",stringise(width),stringise(height));

    setTitle(title);



    memset(bitmapData,0,sizeof(bitmapData));

    theThreads[numThreads] = std::thread{NewtonLowRes};


    for(int i=0;i<numThreads;i++)
    {
        theThreads[i] = std::thread{Newton,i};
    }

}

void go(Point const &topLeft,Point const &bottomRight)
{
    history.emplace(topLeft,bottomRight);
    go();
}


void goUp()
{
    if(history.size() > 1)
    {
        stop();
        history.pop();
        go();
    }
}


void stop()
{
    done=true;
    
    for(auto &thread : theThreads)
    {
        thread.join();
    }
    done=false;
}

int main()
{
    createWindow();

//    go( {-2.0, 1.0}, { 0.5, -1.0} );

    go( {-5.0, 5.0}, { 5.0, -5.0} );
  
    windowMessageLoop();

    stop();
}