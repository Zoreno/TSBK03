/**
* @file		Color.h
* @Author	Joakim Bertils
* @date		2018-10-13
* @brief	Class representing a color.
*/

#pragma once

#include <utility>
#include <iostream>
#include <string>

/**
 * @brief Class representing a color value.
 * 
 * The internal representation is RGBA values as floats ranging from 0.f to
 * 1.f.
 * Other formats are converted to before returned.
 */
class Color
{
	//=========================================================================
	// Friend classes
	//=========================================================================

	//=========================================================================
	// Private internal classes
	//=========================================================================

public:

	//=========================================================================
	// Public typedefs and aliases
	//=========================================================================

	//=========================================================================
	// Constructor
	//=========================================================================

	/**
	 * @brief Default constructor
	 */
	Color();

	/**
	 * @brief Grayscale constructor
	 * 
	 * Initiates all color channels to the same value.
	 * 
	 * @param grayScale Value for color channels.
	 * @param a Value for alpha channel
	 */
	explicit Color(float grayScale, float a = MAX);

	/**
	 * @brief Grayscale constructor using int on range [0, 255].
	 * @param grayScale Int value for color channels.
	 * @param a Int value for alpha channel.
	 */
	explicit Color(int grayScale, int a = INTMAX);

	/**
	 * @brief Color constructor
	 * @param r Value for the red channel.
	 * @param g Value for the green channel.
	 * @param b Value for the blue channel.
	 * @param a Value for the alpha channel.
	 */
	Color(float r, float g, float b, float a = MAX);

	/**
	 * @brief Color constructor using int values.
	 * @param r Int value for the red channel.
	 * @param g Int value for the green channel.
	 * @param b Int value for the blue channel.
	 * @param a Int value for the alpha channel.
	 */
	Color(int r, int g, int b, int a = INTMAX);

	/**
	* @brief Vector constructor from int values.
	*
	* Initiates values from a vector. The values are interpreted as RGBA
	* values if @hsl is false. Otherwise the values are interpretes as
	* HSLA values. All values are in the range [0, 255]
	*
	* @param vec Vector of 4 ints.
	* @param hsl True if the values are HSLA encoded.
	*/
	explicit Color(int *vec, bool hsl = false);

	/**
	 * @brief Vector constructor.
	 * 
	 * Initiates values from a vector. The values are interpreted as RGBA
	 * values if @hsl is false. Otherwise the values are interpretes as
	 * HSLA values.
	 * 
	 * @param vec Vector of 4 floats.
	 * @param hsl True if the values are HSLA encoded.
	 */
	explicit Color(float *vec, bool hsl = false);

	/**
	 * @brief String constructor
	 * 
	 * Creates a color object from a hex string. Accepts 1 to 4 color components
	 * as two char per component hex strings. Leading '#' is optional.
	 * 
	 * #01       creates the color #010101FF
	 * #0123     creates the color #01010123
	 * #012345   creates the color #012345FF
	 * #01234567 creates the color #01234567
	 * 
	 * All other lengths will throw an std::invalid_argument exception.
	 * 
	 * @param str String containing the color hex code.
	 */
	explicit Color(const std::string& str);

	/**
	 * @brief Copy constructor.
	 * @param other The color to copy from.
	 */
	Color(const Color& other);

	/**
	 * @brief Move constructor
	 * @param other The color to move from.
	 */
	Color(Color&& other) noexcept;

	//=========================================================================
	// Destructor
	//=========================================================================

	/**
	 * @brief Destructor.
	 */
	~Color();

	//=========================================================================
	// Assignment operators
	//=========================================================================

	/**
	 * @brief Copy assignment operator.
	 * @param other The object to copy from.
	 * @return Reference to self.
	 */
	Color& operator=(const Color& other) &;

	/**
	 * @brief Move assignment operator.
	 * @param other The object to move from.
	 * @return Reference to self.
	 */
	Color& operator=(Color&& other) & noexcept;

	//=========================================================================
	// Artihmetic operators
	//=========================================================================

	/**
	 * @brief Addition operator.
	 * 
	 * Adds two colors and clamps the value to the valid range.
	 * 
	 * @param rhs The color to add.
	 * @return Color object with the clamped added values.
	 */
	Color operator+(const Color& rhs) const;

	/**
	 * @brief Addition with float operator.
	 * 
	 * Adds a float value to the color, treating the float value as a grayscale
	 * color with an alpha of max value. The result is clamped to the color
	 * range.
	 * 
	 * @param lhs The left hand side of the operator.
	 * @param rhs The right hand side of the operator.
	 * @return Color object with the clamped added values.
	 */
	friend Color operator+(const Color& lhs, float rhs);

	/**
	* @brief Addition to float operator.
	*
	* Adds a color to a float value, treating the float value as a grayscale
	* color with an alpha of max value. The result is clamped to the color
	* range.
	*
	* @param lhs The left hand side of the operator.
	* @param rhs The right hand side of the operator.
	* @return Color object with the clamped added values.
	*/
	friend Color operator+(float lhs, const Color& rhs);

	/**
	 * @brief Subtraction operator.
	 * 
	 * Subtracts a color from another color and clamps the value to the valid
	 * range.
	 * 
	 * @param rhs Color to subtract.
	 * @return Clamped difference between the colors. 
	 */
	Color operator-(const Color& rhs) const;

	/**
	* @brief Subtraction with float operator.
	*
	* Subracts a float value from a color, treating the float value as a 
	* grayscale color with an alpha of min value. The result is clamped to the
	* color range.
	*
	* @param lhs The left hand side of the operator.
	* @param rhs The right hand side of the operator.
	* @return Color object with the clamped subtracted values.
	*/
	friend Color operator-(const Color& lhs, float rhs);

	/**
	* @brief Subtraction from float operator.
	*
	* Subracts color from a float value, treating the float value as a
	* grayscale color. The @rhs alpha value is copied to the result. The result
	* is clamped to the color range. 
	*
	* @param lhs The left hand side of the operator.
	* @param rhs The right hand side of the operator.
	* @return Color object with the clamped subtracted values.
	*/
	friend Color operator-(float lhs, const Color& rhs);

	//=========================================================================
	// Relational operators
	//=========================================================================

	/**
	 * @brief Equality operator.
	 * @param other The object to compare with.
	 * @return True if equal.
	 */
	bool operator==(const Color& other) const;

	/**
	 * @brief Inequality operator.
	 * @param other The object to compare with.
	 * @return True if not equal.
	 */
	bool operator!=(const Color& other) const;

	//=========================================================================
	// Compound Operators
	//=========================================================================

	/**
	 * @brief Compound addition assignment operator.
	 * 
	 * Adds a color to this color. Equivalent to *this = *this + rhs.
	 * 
	 * @param rhs The color to add.
	 * @return Referencet to self.
	 */
	Color& operator+=(const Color& rhs) &;

	/**
	 * @brief Compound addition assignment operator with float.
	 * 
	 * Adds a float value to this color, treating the float value as a
	 * grayscale color with an alpha of max. Equivalent to *this = *this + rhs.
	 * 
	 * @param rhs The float value to add.
	 * @return Reference to self.
	 */
	Color& operator+=(float rhs) &;

	/**
	* @brief Compound subtraction assignment operator.
	*
	* Subtracts a color from this color. Equivalent to *this = *this - rhs.
	*
	* @param rhs The color to subtract.
	* @return Referencet to self.
	*/
	Color& operator-=(const Color& rhs) &;

	/**
	* @brief Compound subtraction assignment operator with float.
	*
	* Subtracts a float value from this color, treating the float value as a
	* grayscale color with an alpha of min. Equivalent to *this = *this - rhs.
	*
	* @param rhs The float value to subtract.
	* @return Reference to self.
	*/
	Color& operator-=(float rhs) &;

	//=========================================================================
	// Iterators
	//=========================================================================

	//=========================================================================
	// Member access
	//=========================================================================

	/**
	 * @brief Red channel getter.
	 * @return Red channel value.
	 */
	float getR() const;

	/**
	 * @brief Green channel getter.
	 * @return Green channel value.
	 */
	float getG() const;
	
	/**
	 * @brief Blue channel getter.
	 * @return Blue channel value.
	 */
	float getB() const;
	
	/**
	 * @brief Alpha channel getter.
	 * @return Alpha channel value.
	 */
	float getA() const;

	/**
	* @brief Red channel getter.
	* @return Red channel value.
	*/
	int getRint() const;

	/**
	* @brief Green channel getter.
	* @return Green channel value.
	*/
	int getGint() const;

	/**
	* @brief Blue channel getter.
	* @return Blue channel value.
	*/
	int getBint() const;

	/**
	* @brief Alpha channel getter.
	* @return Alpha channel value.
	*/
	int getAint() const;

	/**
	 * @brief Red channel setter
	 * @param r New value for the red channel.
	 */
	void setR(float r);

	/**
	* @brief Green channel setter
	* @param g New value for the green channel.
	*/
	void setG(float g);

	/**
	* @brief Blue channel setter
	* @param b New value for the Blue channel.
	*/
	void setB(float b);

	/**
	* @brief Alpha channel setter
	* @param a New value for the alpha channel.
	*/
	void setA(float a);

	/**
	* @brief Red channel setter
	* @param r New value for the red channel.
	*/
	void setRint(int r);

	/**
	* @brief Green channel setter
	* @param g New value for the green channel.
	*/
	void setGint(int g);

	/**
	* @brief Blue channel setter
	* @param b New value for the Blue channel.
	*/
	void setBint(int b);

	/**
	* @brief Alpha channel setter
	* @param a New value for the alpha channel.
	*/
	void setAint(int a);

	/**
	 * @brief Gets the RGB values packed into a float array.
	 * @param vec Array to place the values.
	 */
	void getRGB(float *vec) const;

	/**
	 * @brief Gets the BGR values packed into a float array.
	 * @param vec Array to place the values.
	 */
	void getBGR(float *vec) const;

	/**
	* @brief Converts and gets the HSL values packed into a float array.
	* @param vec Array to place the values.
	*/
	void getHSL(float *vec) const;

	/**
	 * @brief Gets the RGBA values packed into a float array.
	 * @param vec Array to place the values.
	 */
	void getRGBA(float *vec) const;

	/**
	 * @brief Gets the BGRA values packed into a float array.
	 * @param vec Array to place the values.
	 */
	void getBGRA(float *vec) const;

	/**
	* @brief Converts and gets the HSLA values packed into a float array.
	* @param vec Array to place the values.
	*/
	void getHSLA(float *vec) const;

	/**
	* @brief Gets the RGB values packed into a int array.
	* @param vec Array to place the values.
	*/
	void getRGBint(int *vec) const;

	/**
	* @brief Gets the BGR values packed into a int array.
	* @param vec Array to place the values.
	*/
	void getBGRint(int *vec) const;

	/**
	* @brief Converts and gets the HSL values packed into a int array.
	* @param vec Array to place the values.
	*/
	void getHSLint(int *vec) const;

	/**
	* @brief Gets the RGBA values packed into a int array.
	* @param vec Array to place the values.
	*/
	void getRGBAint(int *vec) const;

	/**
	* @brief Gets the BGRA values packed into a int array.
	* @param vec Array to place the values.
	*/
	void getBGRAint(int *vec) const;

	/**
	* @brief Converts and gets the HSLA values packed into a int array.
	* @param vec Array to place the values.
	*/
	void getHSLAint(int *vec) const;

	//=========================================================================
	// Functions
	//=========================================================================

	/**
	 * @brief Interpolates colors.
	 * 
	 * Interpolates colors from a to b by factor t in range [0, 1] in the 
	 * HSLA color model.
	 * 
	 * @param a The first color.
	 * @param b The second color.
	 * @param t Interpolation factor.
	 * @return Result of the operation.
	 */
	friend Color interpolate(const Color& a, const Color& b, float t);

	/**
	 * @brief Outstream operator.
	 * 
	 * Outputs the color as float values.
	 * 
	 * @param stream Reference to stream.
	 * @param c Color to output.
	 * @return Reference to stream.
	 */
	friend std::ostream& operator<<(std::ostream& stream, const Color& c);

	/**
	 * @brief Instream operator.
	 * 
	 * Reads a color value as a quadruple float value from the stream.
	 * 
	 * @param stream Reference to stream.
	 * @param c Color to store the data in.
	 * @return Reference to stream.
	 */
	friend std::istream& operator>>(std::istream& stream, Color& c);

	/**
	 * @brief Outputs the RGBA color in hexadecimal values starting with '#'.
	 * @return String containing the color representation.
	 */
	std::string toString() const;

protected:

	//=========================================================================
	// Protected typedefs and aliases.
	//=========================================================================

	//=========================================================================
	// Protected Members
	//=========================================================================

	//=========================================================================
	// Protected functions
	//=========================================================================

private:

	//=========================================================================
	// Private typedefs and aliases.
	//=========================================================================

	//=========================================================================
	// Private Members
	//=========================================================================

	/**
	* @brief The maximum value for a channel.
	*/
	static constexpr float MAX = 1.f;

	/**
	* @brief The minimum value for a channel.
	*/
	static constexpr float MIN = 0.f;

	/**
	* @brief The maximum value for a channel.
	*/
	static constexpr int INTMAX = 255;

	/**
	* @brief The minimum value for a channel.
	*/
	static constexpr int INTMIN = 0;

	/**
	 * @brief The red channel value.
	 */
	float _r{ MIN };

	/**
	 * @brief The green channel value.
	 */
	float _g{ MIN };

	/**
	 * @brief The blue channel value.
	 */
	float _b{ MIN };

	/**
	 * @brief The alpha channel value.
	 */
	float _a{ MAX };

	//=========================================================================
	// Private functions
	//=========================================================================

	/**
	 * @brief Swaps two color objects.
	 * @param lhs The first color.
	 * @param rhs The second color.
	 */
	friend void swap(Color& lhs, Color& rhs) noexcept;

	/**
	 * @brief Helper to convert a hue value to RGB
	 * @param p Helper variable 1.
	 * @param q Helper variable 2.
	 * @param t Helper variable 3. 
	 * @return Corresponding hue value.
	 */
	static float hueToRgb(float p, float q, float t);

	/**
	 * @brief Clamps a value between @min and @max.
	 * @param v Value to clamp
	 * @param min Minimum value. Defaults to MIN.
	 * @param max Maximum value. Defaults to MAX.
	 * @return min if v < min, max if v > max, else v.
	 */
	static float clamp(float v, float min = MIN, float max = MAX);

	/**
	* @brief Clamps a value between @min and @max.
	* @param v Value to clamp
	* @param min Minimum value. Defaults to MIN.
	* @param max Maximum value. Defaults to MAX.
	* @return min if v < min, max if v > max, else v.
	*/
	static int clamp(int v, int min = INTMIN, int max = INTMAX);
};

//=============================================================================
// End of file.
//=============================================================================