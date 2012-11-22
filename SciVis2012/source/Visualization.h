#ifndef VISUALIZATION_H_INCLUDED
#define	VISUALIZATION_H_INCLUDED

class Simulation;

class Visualization
{
	friend class Application;
	
	public:
		
		Visualization();
		void visualize(Simulation const &simulation, int winWidth, int winHeight);
		
		//different types of color mapping: black-and-white, rainbow, banded
		enum ColorMode
		{
                    Grayscale,
                    Rainbow,
                    Custom,
                    ColorModeCount				// (automatically assigned)
		};
		
		enum Option
		{
			DrawSmoke,				// draw the smoke or not
			DrawVectorField,		// draw the vector field or not
			UseDirectionColoring,	// use direction color-coding or not
			OptionsCount			// (automatically assigned)
		};
		
                void set_scalar_col(ColorMode colorMode);
		void toggle(Option option);
		void enable(Option option);
		void disable(Option option);
		bool is_enabled(Option option) const;		
		void scale_hedgehogs(float scale);
		float hedgehog_scale() const;
		
		void rainbow(float value, float* R, float* G, float* B);
		void set_colormap(float vy);
		void direction_to_color(float x, float y, int method);
		
	private:
		
		float vec_scale;				// scaling of hedgehogs 
		int options[OptionsCount];		// options boolean array
		ColorMode scalar_col;			//method for scalar coloring
};

#endif