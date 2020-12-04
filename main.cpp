//
// C++ graphics demo program, showing how to draw colored squares, text within the squares,
// and allowing you to retrieve current mouse position when the left mouse button is clicked.
//
// Instructions:
//    To run this demo, first select the "Build and Run" option at the top of the window. You
//    can ignore the following error messages that will appear:
//         Failed to use the XRandR extension while trying to get the desktop video modes
//         Failed to use the XRandR extension while trying to get the desktop video modes
//         Failed to initialize inotify, joystick connections and disconnections won't be notified
//    To see the graphical output then select the "Viewer" option at the top of the window.
//
// For more information about SFML graphics, see: https://www.sfml-dev.org/tutorials
// Be sure to close the old window each time you rebuild and rerun, to ensure you
// are seeing the latest output.
//
// Author: Dale Reed
// Class:  UIC CS-141, Spring 2019
// System: C++ on cloud-based Codio.com
//
#include <SFML/Graphics.hpp> // Needed to access all the SFML graphics libraries
#include <iostream>          // For cin, cout, endl
#include <iomanip>           // used for setting output field size using setw
#include <cstdlib>           // For rand()
#include <cstdio>            // For sprintf, "printing" to a string
#include <cstring>           // For c-string functions such as strlen()
#include <chrono>            // Used in pausing for some milliseconds using sleep_for(...)
#include <thread>            // Used in pausing for some milliseconds using sleep_for(...)
using namespace std;


const int WindowYSize = 500;
const int MaxBoardSize = 12;  // Max number of squares per side
const int TileValue = 1024;   // Max tile value to start out on a 4x4 board
const int WindowXSize = 400;


struct Node
{
    int Dupboard[MaxBoardSize * MaxBoardSize];   // declaring all the required ints and variables
    int score;
    Node* pNext;
    int step;
};


//---------------------------------------------------------------------------------------
class Square {
public:
    // Default Constructor
    Square()
    {
        size = 0;
        xPosition = 0;
        yPosition = 0;
        color = sf::Color::Black;
        isVisible = false;
        isCaptured = false;
        text = "";
    }
    
    // Fully-qualified constructor, used to set all fields
    Square( int theSize, int theXPosition, int theYPosition,
           const sf::Color &theColor, bool theVisibility, std::string theText)
    {
        // Set the class fields
        size = theSize;
        xPosition = theXPosition;
        yPosition = theYPosition;
        color = theColor;
        isVisible = theVisibility;
        isCaptured = false;   // By default squares have not been captured
        text = theText;
        // Use the values to set the display characteristics of theSquare
        theSquare.setSize( sf::Vector2f(theSize, theSize));
        theSquare.setPosition( theXPosition, theYPosition);   // Set the position of the square
        theSquare.setFillColor( theColor);
    }
    
    // Get (accessor) functions
    sf::RectangleShape getTheSquare() { return theSquare; }
    int getSize() { return size; }
    int getXPosition() { return xPosition; }
    int getYPosition() { return yPosition; }
    sf::Color& getColor() { return color; }
    bool getIsVisible() { return isVisible; }
    bool getIsCaptured() { return isCaptured; }
    std::string getText() { return text; }
    
    // Set (mutator) functions
    void setSize( int theSize) {
        size = theSize;
        theSquare.setSize( sf::Vector2f(theSize, theSize));
    }
    void setXPosition( int theXPosition) {
        xPosition = theXPosition;
        theSquare.setPosition( theXPosition, yPosition);   // Set the position of the square
    }
    void setYPosition( int theYPosition) {
        yPosition = theYPosition;
        theSquare.setPosition( xPosition, theYPosition);   // Set the position of the square
    }
    void setColor( sf::Color & theColor) {
        color = theColor;
        theSquare.setFillColor( theColor);    // Also update the color on the square itself
    }
    void setColor( int R, int G, int B) {
        sf::Color theNewColor( R, G, B);
        color = theNewColor;
        theSquare.setFillColor( theNewColor);
    }
    void setVisibility( bool theVisibility) { isVisible = theVisibility; }
    void setIsCaptured( bool isCaptured) { this->isCaptured = isCaptured; }
    void setText( std::string theText) { text = theText; }
    
    // Utility functions
    void displayText( sf::RenderWindow *pWindow, sf::Font theFont, sf::Color theColor, int textSize);
    
private:
    int size;
    int xPosition;
    int yPosition;
    sf::Color color;
    bool isVisible;
    bool isCaptured;   // Indicates whether or not it is part of the captured area
    std::string text;
    sf::RectangleShape theSquare;
    
}; //end class Square


void Square::displayText(
                         sf::RenderWindow *pWindow,   // The window into which we draw everything
                         sf::Font theFont,            // Font to be used in displaying text
                         sf::Color theColor,          // Color of the font
                         int textSize)                // Size of the text to be displayed
{
    // Create a sf::Text object to draw the text, using a sf::Text constructor
    sf::Text theText( text,        // text is a class data member
                     theFont,     // font from a font file, passed as a parameter
                     textSize);   // this is the size of text to be displayed
    
    // Text color is the designated one, unless the background is Yellow, in which case the text
    // color gets changed to blue so we can see it, since we can't see white-on-yellow very well
    if( this->getColor() == sf::Color::Yellow) {
        theColor = sf::Color::Blue;
    }
    theText.setColor( theColor);
    
    // Place text in the corresponding square, centered in both x (horizontally) and y (vertically)
    // For horizontal center, find the center of the square and subtract half the width of the text
    int theXPosition = xPosition + (size / 2) - ((strlen(text.c_str()) * theText.getCharacterSize()) / 2);
    // For the vertical center, from the top of the square go down the amount: (square size - text size) / 2
    int theYPosition = yPosition + (size - theText.getCharacterSize()) / 2;
    // Use an additional offset to get it centered
    int offset = 5;
    theText.setPosition( theXPosition + offset, theYPosition - offset);
    
    // Finally draw the Text object in the RenderWindow
    pWindow->draw( theText);
}


//---------------------------------------------------------------------------------------
// Initialize the font
void initializeFont( sf::Font &theFont)
{
    // Create the global font object from the font file
    if (!theFont.loadFromFile("arial.ttf"))
    {
        std::cout << "Unable to load font. " << std::endl;
        exit( -1);
    }
}//--------------------------------------------------------------------
// Display Instructions
void displayInstructions()
{
    std::cout << "Welcome to 1024. \n"
    << "  \n"
    << "For each move enter a direction as a letter key, as follows: \n"
    << "    W    \n"
    << "  A S D  \n"
    << "where A=left,W=up, D=right and S=down. \n"
    << "  \n"
    << "After a move, when two identical valued tiles come together they    \n"
    << "join to become a new single tile with the value of the sum of the   \n"
    << "two originals. This value gets added to the score.  On each move    \n"
    << "one new randomly chosen value of 2 or 4 is placed in a random open  \n"
    << "square.  User input of x exits the game.                            \n"
    << "  \n";
}//end displayInstructions()

void Random1( int game[], int shapes)
{
    
    int Pos = 2;
    if( rand()%2 == 1) {      // for randomly selecting 2 or 4
        Pos = 4;
    }
    
    int temp1;
    do {
        temp1 = rand() % (shapes*shapes);
    }while( game[ temp1] != 0);
    
    // blank index position
    game[ temp1] = Pos;
}

//--------------------------------------------------------------------

void BoardSet(
              int play[],           
              int BoardPrim[],   
              int &blocks,   
              int &Tile)
{
    //  Initialization the array of int values used to represent the Ascii board
   
    for( int t =0; t <blocks; t++) {
        for( int y =0; y <blocks; y++ ) {
            play[ t*blocks + y] = 0;
        }
    }
    
    // Displaying and Calculatinng game value
    Tile = TileValue;        
    for( int r=4; r<blocks; r++) {
        Tile = Tile * 2;   
    }
    std::cout << "Game ends when you reach ";
    std::cout << Tile << ".";
    std::cout <<std::endl;
    
    // To Set two random pieces for starting the game
    Random1( play, blocks);
    Random1( play, blocks);
}



void BoardVisual( int play[], int block, int score)
{
    std::cout << "\n";
    std::cout << "Score: ";
    std::cout << score <<std::endl;
    for( int x =0; x <block; x++) {
        std::cout << "   ";
        for( int y=0; y<block; y++ ) {    // to display the board on the screen
            int temp1 = x*block + y;  
            std::cout << std::setw( 6);    
            
            if( play[ temp1] == 0) {
                std::cout << '.';
            }
            else {
                std::cout << play[ temp1];
            }
        }
        std::cout << "\n\n";
    }
}


// To Make a board copy .Used after an attempted move
// to see if the board actually changed.
void duplicate(
               int BoardOld[], 
               int play[],         
               int blocks)       
{
    for( int g =0; g <blocks; g++) {
        for( int h=0; h<blocks; h++ ) {
            int temp2 = g*blocks + h;  
            BoardOld[ temp2] = play[ temp2];
        }
    }
}


// Returning true if boards are different or false.
bool Board1( int BoardOld[], int play[], int squaresPerSide)
{
    // To Compare the element by element.  If one is found that is different
    // then return true, as board was changed.
    for( int x =0; x <squaresPerSide; x++) {
        for( int c=0; c<squaresPerSide; c++ ) {
            int change = x*squaresPerSide + c;  
            if( BoardOld[ change] != play[ change]) {
                return true;
            }
        }
    }
    
    return false;  
}



void appendNode(Node* &pHead, int game[], int score, int step, int temp2)
{
    Node* pTemp = new Node; // create new node
    duplicate(pTemp->Dupboard, game, temp2); // store the board in the new node
    pTemp->score = score; // For storing the score in the new node
    pTemp->step = step; // For storing the move number in the new node
    pTemp->pNext = pHead; 
    pHead = pTemp; 
    
} 




void slideRight( int play[], int blocks, int &score)
{
    
    for( int e=0; e<blocks; e++) {
       
        int limit = e * blocks + blocks - 1;
        
        
        for( int col=blocks - 1; col>=0; col--) {
            
            // get 1-d array index based on row and col
            int current = e * blocks + col;
            
            // slide current piece over as far right as possible
            while( current < limit && play[ current+1] == 0) {
                play[ current+1] = play[ current];
                play[ current] = 0;
                current++;
            }
            
           
            if( (current < limit) && (play[ current+1] == play[ current]) && (play[ current] != 0) ) {
                play[ current+1] = play[ current+1] + play[ current];
                play[ current] = 0;
                limit = current;           // Reset row index limit, to prevent combining a piece more than once
                score += play[ current+1];  // Update score
            }
            
        }//end for( int col...
    }//end for( int row...
    
}//end slideRight()


//--------------------------------------------------------------------
// Slide all tiles up, combining matching values, updating the score
void Up1( int board[], int squaresPerSide, int &score)
{
    // handle each column separately
    for( int col=0; col<squaresPerSide; col++) {
        // set index limit for this column to be index of top-most tile on this row
        int limit = col;
        
        
        for( int row=1; row<squaresPerSide; row++) {
            
            
            int current = row * squaresPerSide + col;
            
           
            while( (current > limit) && (board[ current-squaresPerSide] == 0) ) {
                board[ current-squaresPerSide] = board[ current];
                board[ current] = 0;
                current = current - squaresPerSide;
            }
            
            
            if( (current > limit) && (board[ current-squaresPerSide] == board[ current]) && (board[ current] != 0) ) {
                board[ current-squaresPerSide] = board[ current-squaresPerSide] + board[ current];
                board[ current] = 0;
                limit = current;           // Reset row index limit, to prevent combining a piece more than once
                score += board[ current-squaresPerSide];  // Update score
            }
            
        }//end for( int col...
    }//end for( int row...
    
}//end slideUp()

void erase(Node* &pHead, int game[], int &score, int &step, int temp2)
{
    Node* pTemp = pHead; // create a new pointer and make it point to the
    // head of the list
    pHead = pHead->pNext; 
    delete pTemp; 
    duplicate(game, pHead->Dupboard,temp2); // change the contens of the board
    score = pHead->score; // change the contents of score
    step = pHead->step; 
} 

//--------------------------------------------------------------------
// Slide all tiles down, combining matching values, updating the score
void Down1( int play[], int squaresPerSide, int &score)
{
    // handle each column separately
    for( int col=0; col<squaresPerSide; col++) {
        // set index limit for this column to be index of bottom-most tile on this row
        int limit = (squaresPerSide - 1) * squaresPerSide + col;
        
        // Start from the next to last row and process each element from bottom to top
        for( int row=squaresPerSide-1; row>=0; row--) {
            
            // get 1-d array index based on row and col
            int current = row * squaresPerSide + col;
            
            // slide current piece down as far as possible
            while( current < limit && play[ current+squaresPerSide] == 0 ) {
                play[ current+squaresPerSide] = play[ current];
                play[ current] = 0;
                current = current + squaresPerSide;
            }
            
            // Combine it with lower neighbor if values are the same and non zero.
            // The additional check for (current < limit) ensures a tile can be combined
            // at most once on a move, since limit is moved up every time a combination is made.
            if( (current < limit) && (play[ current+squaresPerSide] == play[ current]) && (play[ current] != 0) ) {
                play[ current+squaresPerSide] = play[ current+squaresPerSide] + play[ current];
                play[ current] = 0;
                limit = current;           // Reset row index limit, to prevent combining a piece more than once
                score += play[ current+squaresPerSide];  // Update score
            }
            
        }//end for( int col...
    }//end for( int row...
    
}//end slideDown()
void Left1( int game[], int shapes, int &score)
{
    // handle each row separately
    for( int s=0; s<shapes; s++) {
        // set index limit for this row to be index of left-most tile on this row
        int limit = s * shapes;
        
        // Start from the second column and process each element from left to right
        for( int r=1; r<shapes; r++) {
            
            // get 1-d array index based on row and col
            int current = s * shapes + r;
            
            // slide current piece over as far left as possible
            while( current > limit && game[ current-1] == 0) {
                game[ current-1] = game[ current];
                game[ current] = 0;
                current--;
            }
            
            // Combine it with left neighbor if values are the same and non zero.
            // The additional check for (current > limit) ensures a tile can be combined
            // at most once on a move, since limit is moved right every time a combination is made.
            // This ensures a row of:  2 2 4 4   ends up correctly as:  4 8 0 0   and not:  8 4 0 0
            if( (current > limit) && (game[ current-1] == game[ current]) && (game[ current] != 0) ) {
                game[ current-1] = game[ current-1] + game[ current];
                game[ current] = 0;
                limit = current;           // Reset row index limit, to prevent combining a piece more than once
                score += game[ current-1];  // Update score
            }
            
        }//end for( int col...
    }//end for( int row...
    
}//end slideLeft()




bool gameEnds( int play[],      // current play
              int blocks,    // size of one side of board
              int Tile) // max tile value for this size board
{
    // See if the Tile2 is found anywhere on the board.
    // If so, game is over.
    for( int d =0; d <blocks*blocks; d ++) {
        if( play[ d] == Tile) {
            std::cout << "Congratulations!  You made it to ";
            std::cout << Tile << " !!!" <<std::endl;
            return true;  // game is over
        }
    }
    
    // See if there are any open squares.  If so return true since we aren't done
    for( int d=0; d<blocks*blocks; d++) {
        if( play[ d] == 0) {
            return false;  // there are open squares, so game is not over
        }
    }
    
    // All squares are full.
    // To check if board is done, make a copy of board, then slide left
    // and slide down.  If resulting board is same as original, we are done
    // with game since there are no moves to be made.
    int Dupboard[ blocks * blocks];
    duplicate( Dupboard, play, blocks);
    int num5 = 0;   // used as a placeHolder only for function calls below
    Left1( Dupboard, blocks, num5);
    Down1( Dupboard, blocks, num5);
    
    // Compare each square of boards.  If any is different, a move is possible
    for( int i=0; i<blocks*blocks; i++) {
        if( Dupboard[ i] != play[ i]) {
            return false;  // Game is not over
        }
    }
    
    std::cout << "\n";
    std::cout << "No more available moves.  ";
    std::cout << "Game is over.\n";
    std::cout << "\n";
    return true;  // Game is over since all squares are full and there are no moves
}//end gameEnds()

int main()
{
    int arraySize = 4;
    int move = 1;              
    int score = 0;                    
    int squaresPerSide = 4;           // User will enter this value.  Set default to 4
    char Input = ' ';     // Stores user input
    char sent[ 81];        // C-string to hold concatenated output of character literals
    int var1 = 0, var2 = 0;                                                                                                //    if a move changed the board.
    int board[ MaxBoardSize * MaxBoardSize];         
    int BoardPrim[ MaxBoardSize * MaxBoardSize];  
    int Tile2 = TileValue;  
    
    Square squaresArray[ MaxBoardSize * MaxBoardSize];
    
    // Create and initialize the font, to be used in displaying text.
    sf::Font font;
    initializeFont( font);
   
    sf::RenderWindow window(sf::VideoMode(WindowXSize, WindowYSize), "Program 5: 1024");
    
    // Create the messages label at the bottom of the screen, to be used in displaying debugging information.
    sf::Text messagesLabel( "Welcome to 1024", font, 24);
    // Make a text object from the font
    messagesLabel.setColor(sf::Color::White);
    // Place text at the bottom of the window. Position offsets are x,y from 0,0 in upper-left of window
    messagesLabel.setPosition( 0, WindowYSize - messagesLabel.getCharacterSize() - 5);
    
    displayInstructions();
    
    // Get the board size, create and initialize the board, and set the max tile value
    BoardSet( board, BoardPrim, squaresPerSide, Tile2);
    
    Node* pHead = NULL;
    
    // creating a new node to store the initial board information in the list
    Node* pTemp = new Node;
    duplicate(pTemp->Dupboard, board, squaresPerSide); // storing a copy of the board
    pTemp->step = move; // storing the initial move number
    pTemp->score = score; // storing the score in the game
    
    pTemp->pNext = NULL;  // moving to the next node
    pHead = pTemp; // make the pHead point to the node
    
    
    
    while (window.isOpen())
    {
        //             char c='P';
        var1 = 0;
        var2 = 0;
        
        for(int num2 = 0; num2 < squaresPerSide * squaresPerSide; num2++)
        {
           
            char ident[ 81];
            // Squares with a 0 value should not have a number displayed
            if( board[ num2] == 0)
            {
                strcpy( ident, "");   // "print" a blank text string
            } // if( board[ i] == 0)
            else
            {
                sprintf( ident, "%d", board[ num2]);   // "print" the square creation order number into a string to be stored in the square
            } // else ends
            // Set each array pointer have the address of a new Square, created with a Square constructor
            squaresArray[ num2] = Square(55, 55*var1 + var1*10, 55*var2 + var2*10, sf::Color::White, true, ident);
            // Draw the square
            window.draw( squaresArray[ num2].getTheSquare());
            // Draw the text associated with the Square, in the window with the indicated color and text size
            int red = 0, green = 0, blue = 0; // initialize the RGB
            squaresArray[num2].displayText( &window, font, sf::Color(red, green, blue), 30);
            
            // check if we have reached the end of the row
            if((num2 + 1) % 4 == 0)
            {
                var1 = 0;
                var2 ++;
            } // if((i + 1) % 4 == 0) ends
            else
            {
                var1++;
            } // else ends
        } // for i ends
        
        // Construct string to be displayed at bottom of screen
        sprintf( sent, "Move %d", move);          
        messagesLabel.setString( sent);            // Store the string into the messagesLabel
        window.draw( messagesLabel);                  // Display the messagesLabel
       
        window.display();
        
        //defining pHead and initialising it to NULL
        
        // Display both the graphical and text boards.
        BoardVisual( board, squaresPerSide, score);
        
        std::cout << "List: ";
        for(int x = move; x > 0; x--)
        {
            if(x != 1)
                std::cout << x << "->";
            else
                std::cout << "1";
        } // for x ends
        std::cout << endl;
        std::cout << endl;
        
        duplicate( BoardPrim, board, squaresPerSide);  // copy from board into BoardPrim
        
        // Prompt for and handle user input
        std::cout << move << ". Your move: ";
        std::cin >> Input;
        switch (Input) {
            case 'x':
                std::cout << "Thanks for playing.";
                std::cout << " Exiting program... \n\n";
                exit( 0);
                break;
            case 'u':
                // Undo the move to the previous move
                if(pHead->step == 1)
                {
                    std::cout << "*** You cannot undo past ";
                    std::cout << "the beginning of the game.  ";
                    std::cout << "Please retry. ***" << endl;
                    continue;
                } // if(pHead->step == 1) ends
                else
                {
                    std::cout << "* Undoing move *" << endl;
                    erase(pHead, board, score, move, squaresPerSide);
                } // else ends
                window.clear();
                continue;
                break;
                
            case 'r':
                std::cout << "\n"
                << "Resetting board \n"
                << "\n";
                // Prompt for board size
                std::cout << "Enter the size board you want, between 4 and 12: ";
                move >> squaresPerSide;
                std:: cin>>squaresPerSide;
                BoardSet( board, BoardPrim, squaresPerSide, Tile2);
                score = 0;
                move = 1;
                continue;  
                break;
            case 'd':
                slideRight( board, squaresPerSide, score); // Slide right
                break;
            case 'a':
                Left1( board, squaresPerSide, score);  // Slide left
                break;
            case 'p':
                
                int temp4;  // 1-d array index location to place piece
                int temp5;  // value to be placed
                std::cin >> temp4 >> temp5;
                board[ temp4] = temp5;
                continue;  // Do not increment move number or place random piece
                break;
                
            case 's':
                Down1( board, squaresPerSide, score);  // Slide down
                break;
                
            case 'w':
                Up1( board, squaresPerSide, score);    // Slide up
                break;
                
            default:
                std::cout << "Invalid input,";
                std::cout <<" please retry.";
                continue;
                break;
        }//end switch( Input)
       
        
        if( Board1( BoardPrim, board, squaresPerSide)) {
            // Place a random piece on board
            Random1( board, squaresPerSide);
            
            // Update move number after a valid move
            move++;
            // Prepending a new node in the list
            appendNode(pHead, board, score, move, squaresPerSide);
        }
        
        // See if we're done
        if( gameEnds( board, squaresPerSide, Tile2)) {
            // Display the final board
            BoardVisual( board, squaresPerSide, score);
            break;
        }
        
        // Clear the screen
        window.clear();
        
        // Pause the event loop, so that Codio does not think it is a runaway process and kill it after some time
        std::this_thread::sleep_for(std::chrono::milliseconds( 10));
        
    }//end while( window.isOpen())
    
    return 0;
}//end main()
