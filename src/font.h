

typedef struct Font {
    const char* name;
    const char* description;
    unsigned char data[2048];
} Font;


extern const Font font__msx;
extern const Font font__msx_din;

