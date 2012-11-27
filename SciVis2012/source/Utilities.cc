int clamp(float x)
{ 
	return ((x)>=0.0?((int)(x)):(-((int)(1-(x))))); 
}

void hsv2rgb(float h, float s, float v, float& r, float& g, float& b)
{
    int hueCase = (int)(h*6);
    float frac = 6*h-hueCase;
    float lx = v*(1-s);
    float ly = v*(1-s*frac);
    float lz = v*(1-s*(1-frac));
    switch (hueCase)
    {
        case 0:
        case 6: r=v;    g=lz;   b=lx;   break;
        case 1: r=ly;   g=v;    b=lx;   break;
        case 2: r=lx;   g=v;    b=lz;   break;
        case 3: r=lx;   g=ly;   b=v;    break;
        case 4: r=lz;   g=lx;   b=v;    break;
        case 5: r=v;    g=lx;   b=ly;   break;
    }
}