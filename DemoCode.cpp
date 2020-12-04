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
#include <iostream>          // Since we are using multiple libraries, now use std::
                             // in front of every cin, cout, endl, and string 
#include <cstdio>            // For sprintf, "printing" to a string
#include <cstring>           // For c-string functions such as strlen() 
#include <chrono>            // Used in pausing for some milliseconds using sleep_for(...)
#include <thread>            // Used in pausing for some milliseconds using sleep_for(...)

const int WindowXSize = 400;
const int WindowYSize = 500;


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


//---------------------------------------------------------------------------------------
// Square class utility function to create a sf::Text object to store and display text 
// associated with this Square.
// 
// Assuming we display output in sf::RenderWindow window(...), then call this function using: 
//    aSquare.displayTest( &window);
// or when using an array of Square pointers declared as:  Square *squaresArray[ 4];
// then call it using:  squaresArray[i]->displayText( &window);
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
}


//--------------------------------------------------------------------
// Display Instructions
void displayInstructions()
{
    std::cout << "   \n"
			  << "Welcome to 1024. \n"
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


//--------------------------------------------------------------------
// Place a randomly selected value of 2 or 4 into a random open square 
// on the board.
void placeRandomPiece( int board[], int squaresPerSide)
{
    // Randomly choose a piece to be placed (2 or 4)
    int pieceToPlace = 2;
    if( rand()%2 == 1) {
        pieceToPlace = 4;
    }
    
    // Find an unoccupied square that currently has a 0
    int index;
    do {
        index = rand() % (squaresPerSide*squaresPerSide);
    }while( board[ index] != 0);
    
    // board at position index is blank, so place piece there
    board[ index] = pieceToPlace;
}//end placeRandomPiece()


//---------------------------------------------------------------------------------------
int main()
{	
	char aString[ 81];    // used to create a string to display text in a square 
	int board[ 4];        // Board of 4 integer values
	
	// Create and initialize the font, to be used in displaying text.
	sf::Font font;  
	initializeFont( font);

	// Create the destination window
	sf::RenderWindow window(sf::VideoMode(WindowXSize, WindowYSize), "Program 5: 1024");
	
	// Create the messages label at the bottom of the screen, to be used in displaying debugging information.
	sf::Text messagesLabel( "Welcome to 1024", font, 24);
	// Make a text object from the font
	messagesLabel.setColor(sf::Color::Red);
	// Place text at the bottom of the window. Position offsets are x,y from 0,0 in upper-left of window
	messagesLabel.setPosition( 0, WindowYSize - messagesLabel.getCharacterSize() - 5);  
	
	displayInstructions();
	
	// Initialize numeric values for board array of integers to all be 0
	for( int i=0; i<4; i++) {
		board[ i] = 0;
	}
	// Set two randomly selected board positions to have a value 2 or 4 in it.
	placeRandomPiece( board, 4);
	placeRandomPiece( board, 4);
	
	// Create array of Square objects
	int arraySize = 4;
	Square squaresArray[ arraySize];
	for( int i=0; i<arraySize; i++) {
		// Allocate each square individually.
		// Store a string in each square which contains its number, in order of creation
		char name[ 81];
		// Squares with a 0 value should not have a number displayed
		if( board[ i] == 0) {
			strcpy( name, "");   // "print" a blank text string
		}
		else {
		   sprintf( name, "%d", board[ i]);   // "print" the square creation order number into a string to be stored in the square
		}
		// Set each array pointer have the address of a new Square, created with a Square constructor
		squaresArray[ i] = Square(50, 50 * i + i*10, 0, sf::Color::Red, true, name);
		// Draw the square
		window.draw( squaresArray[ i].getTheSquare());
		// Draw the text associated with the Square, in the window with the indicated color and text size
		int red = 255, green = 255, blue = 255;
		squaresArray[i].displayText( &window, font, sf::Color(red, green, blue), 30);
	}
	// Display the background frame buffer, replacing the previous RenderWindow frame contents.
	// This is known as "double-buffering", where you first draw into a background frame, and then
	// replace the currently displayed frame with this background frame.
	window.display();
	
	// Run the program as long as the window is open.  This is known as the "Event loop".
	while (window.isOpen())
	{
		char c;
		std::cout << "Enter x to exit or a number 1..3 to continue: ";
		std::cin >> c;
		if( c=='x') {
			std::cout << "Exiting program. \n" << std::endl;
			window.close();
			exit( 0);
		}
		int squareToChange = c - '0';  // Convert input character to numerical value 0..3
		squaresArray[ squareToChange].setColor( 0,0,255);   // Only blue
		// Blank out the text on the selected squaresArray
		squaresArray[ squareToChange].setText( "0");

		// Clear the graphics window, erasing what is displayed
		window.clear();

		// Redraw all screen components to the background frame buffer
		for( int i=0; i<arraySize; i++) {
			// Draw the square
			window.draw( squaresArray[ i].getTheSquare());
			// Draw the text associated with the Square, in the window with the indicated color and text size
			int red = 255, green = 255, blue = 255;
			squaresArray[i].displayText( &window, font, sf::Color(red, green, blue), 30);
		}

		// Construct string to be displayed at bottom of screen
		sprintf( aString, "Changing square %c", c);   // Print into aString
		messagesLabel.setString( aString);            // Store the string into the messagesLabel
		window.draw( messagesLabel);                  // Display the messagesLabel
		
		// Display the background frame buffer, replacing the previous RenderWindow frame contents.
		// This is known as "double-buffering", where you first draw into a background frame, and then
		// replace the currently displayed frame with this background frame.
		window.display();
		
		system("clear");   // Clear the screen 

		// Pause the event loop, so that Codio does not think it is a runaway process and kill it after some time
		std::this_thread::sleep_for(std::chrono::milliseconds( 10));
		
	}//end while( window.isOpen())
	
	return 0;
}//end main()