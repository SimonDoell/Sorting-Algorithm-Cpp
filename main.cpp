#include <SFML/Graphics.hpp>
#include <list>
#include <vector>
#include <iostream>
#include <cmath>
#include <random>
#include <cstdlib>
#include <ctime>
#include <omp.h>
#include <chrono>
#include <iomanip>


//Window Management
const int WIDTH = 1920;
const int HEIGHT = 1010;
int MAX_FRAMES = 60;
bool windowHasFocus;


//Array Management
float maxValue = 100.0f;
bool allSorted = false;



bool performIterationsOnButton = true;
int iterationsPerButtonFrame = 1;
sf::Keyboard::Key triggerKey = sf::Keyboard::Space;


bool performIterationsOnFrame = false;
int iterationsPerFrame = 1;


sf::Keyboard::Key toggleFastSortKey = sf::Keyboard::H;
bool isFastSortEnabled = false;




void exchangeValueByIndexInArray(std::vector<float>& array,  int index1, int index2) {
    float tempVal = array[index1];
    array[index1] = array[index2];
    array[index2] = tempVal;
}


float detail = 10000;


void initialiseRandValues(std::vector<float>& array) {
    int arraySize = array.size();
    for (int i = 0; i < arraySize; ++i) {
        array[i] = float(maxValue) / (float(arraySize) / float(arraySize - i + 1));
    }

    for (int i = 0; i < arraySize; ++i) {
        exchangeValueByIndexInArray(array, i, rand() % arraySize);
    }
}



struct sortingArray {
    private:
        int arraySize;
        std::vector<float> array;
        

        std::string sortAlgoName;

        //Other
        float topPadding = 50.0f;
        float sidePadding = 50.0f;


        //Draw objs and vars
        sf::RectangleShape column;
        sf::Vector2f pos = sf::Vector2f(0, 0);
        sf::Vector2f size = sf::Vector2f(WIDTH, HEIGHT);


        //Sorting
        int currentSortIndex = 0;

        

    public:
        bool isSorted = false;

        //Constructing
        sortingArray(int _arraysize, sf::Vector2f _pos, sf::Vector2f _size, std::string _name) : arraySize(_arraysize), size(_size), pos(_pos), sortAlgoName(_name) {
            array.resize(arraySize);
            initialiseRandValues(array);
        }



        void render(sf::RenderWindow& window) {
            if (isSorted) {
                column.setFillColor(sf::Color::Red);
            } else {
                column.setFillColor(sf::Color::White);
            }

            float columnWidth = size.x/arraySize;

            for (int i = 0; i < arraySize; ++i) {
                
                column.setSize(sf::Vector2f(columnWidth, -array[i] / maxValue * size.y));
                column.setPosition(sf::Vector2f(columnWidth * i + pos.x, size.y + pos.y));
                window.draw(column);
            }
        }

        void performSortCheck() {
            isSorted = true;

            for (int i = 0; i < arraySize-1; ++i) {
                if (array[i] <= array[i+1]) {
                    continue;
                } else {
                    isSorted = false;
                }
            }
        }



        void sort() {
            if (!isSorted) {
                if (sortAlgoName == "Selection Sort") {
                    selectionSortStep();
                } else if (sortAlgoName == "Bubble Sort") {
                    bubbleSortStep();
                }
            }
        }



        void bubbleSortStep() {
            for (int i = 0; i < arraySize-1; ++i) {
                if (array[i] > array[i+1]) {
                    exchangeValueByIndexInArray(array, i, i+1);
                }
                
            }
        }

        void selectionSortStep() {
            float currentArrayValue = array[currentSortIndex];
            int exchangeIndex = currentSortIndex;

            for (int i = currentSortIndex+1; i < arraySize; ++i) {
                float compareValue = array[i];

                if (currentArrayValue > compareValue) {
                    currentArrayValue = compareValue;
                    exchangeIndex = i;
                }
            }

            exchangeValueByIndexInArray(array, currentSortIndex, exchangeIndex);



            currentSortIndex++;
        }



        void reset() {
            currentSortIndex = 0;
            initialiseRandValues(array);
        }
};






int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Sorting Algorithms", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(MAX_FRAMES);
    sf::Event ev;
    sf::ContextSettings settings;
    sf::View view(sf::FloatRect(0, 0, WIDTH, HEIGHT));
    view.zoom(1.0f);
    window.setView(view);
    srand(time(0));
    sf::Clock deltaClock;
    bool toggleDown = false;


    //Arrays
    std::vector<sortingArray> arrays;

    arrays.emplace_back(sortingArray(WIDTH, sf::Vector2f(0, HEIGHT/2.0f), sf::Vector2f(WIDTH, HEIGHT/2.0f), "Selection Sort"));
    arrays.emplace_back(sortingArray(WIDTH, sf::Vector2f(0, 0), sf::Vector2f(WIDTH, HEIGHT/2.0f), "Bubble Sort"));


    int fastSortIterationCount = 5;



    while (window.isOpen()) {
        while (window.pollEvent(ev))
        {
            switch(ev.type) 
            {
            case sf::Event::Closed:
                window.close();
                break;
            }
            if (ev.type == sf::Event::LostFocus) {
                windowHasFocus = false;
            } else if (ev.type == sf::Event::GainedFocus) {
                windowHasFocus = true;
            }
        }
        //INTERACTIONS
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

        if (sf::Keyboard::isKeyPressed(triggerKey) && performIterationsOnButton) {
            for (int i = 0; i < iterationsPerButtonFrame; ++i) {
                for (int index = 0; index < arrays.size(); ++index) {
                    if (!arrays[index].isSorted) {
                        arrays[index].sort();
                    }
                }
            }
        }


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
            for (int i = 0; i < arrays.size(); ++i) {
                arrays[i].reset();
            }
        }


        if (sf::Keyboard::isKeyPressed(toggleFastSortKey)) {
            if (!toggleDown) {
                isFastSortEnabled = !isFastSortEnabled;
            }
            toggleDown = true;
        } else {
            toggleDown = false;
        }







        //RENDERING
        window.clear(sf::Color::Black);


        //sorting
        if (performIterationsOnFrame) {
            for (int i = 0; i < iterationsPerFrame; ++i) {
                for (int index = 0; index < arrays.size(); ++index) {
                    if (!arrays[index].isSorted) {
                        arrays[index].sort();
                    }
                }
            }
        }

        //Checking if solved
        for (int index = 0; index < arrays.size(); ++index) {
            arrays[index].performSortCheck();
        }

        //Updating the allSorted var
        allSorted = true;
        for (int i = 0; i < arrays.size(); ++i) {
            if (!arrays[i].isSorted) {
                allSorted = false;
            }
        }

        if (allSorted) {
            isFastSortEnabled = false;
        }

        if (isFastSortEnabled) {
            for (int i = 0; i < arrays.size(); ++i) {
                for (int it = 0; it < fastSortIterationCount; ++it) {
                    arrays[i].sort();
                }
            }
        }


        for (int index = 0; index < arrays.size(); ++index) {
            arrays[index].render(window);
        }

        window.display();
    }
    return 0;
}
