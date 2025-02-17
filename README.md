
PhoXo Core Lib
===========
**PhoXo Core Lib** is a lightweight, header-only C++ library designed for image manipulation. Originally developed as a cross-platform solution, the library has since shifted focus to Windows due to limited time for maintenance and the desire to optimize performance on this platform. Key enhancements for Windows include features such as thread pooling, Bitmap handles, and seamless integration with Device Contexts (DC) and Direct2D (D2D).

Despite this shift, the core of the library remains focused on image processing, which fundamentally involves manipulating two-dimensional arrays. The majority of the code is written in generic C++, ensuring flexibility and efficiency. I hope you find the information here useful.

You can also find the original version online : [PhoXo Core Lib legacy](https://www.codeproject.com/Articles/13559/ImageStone)

## Prepare to Use
> **For MFC applications**
- Include the header: `#include "phoxo_core.h"`
- call **`phoxo::CoreLib::Init()`** at the program's entry point
- call **`phoxo::CoreLib::Uninit()`** at the program's exit point

A typical initialization code looks like this:
```c++
BOOL CPhoXoSeeApp::InitInstance()
{
    phoxo::CoreLib::Init();

    InitMFCStandardCode();
    __super::InitInstance();

    return FALSE;
}

int CPhoXoSeeApp::ExitInstance()
{
    phoxo::CoreLib::Uninit();
    return __super::ExitInstance();
}
```
> **For pure SDK applications**
- Include `<atlstr.h>` and `<atltypes.h>` **before** including `phoxo_core.h`
```c++
#include <atlstr.h>
#include <atltypes.h>
#include "phoxo_core_lib/src/phoxo_core.h"
using namespace phoxo;
```

## Load / Save image file 
> **Load image from File**
```c++
Image   img;
// load an image using GDI+
Gdiplus::Bitmap   src(L"d:\\a.jpg");
CodecGdiplus::Load(src, img);

// load an image using WIC with premultiplied alpha format
CodecWIC::LoadFile(L"d:\\a.jpg", img, WICPremultiplied32bpp);
```

> **Load image from Memory**
```c++
auto   stream = Utils::CreateMemStream(buf, buf_size);

Image   img;
// load an image using GDI+
Gdiplus::Bitmap   src(stream);
CodecGdiplus::Load(src, img);

// load an image using WIC with premultiplied alpha format
CodecWIC::LoadStream(stream, img, WICNormal32bpp);
```

> **Save image to File**
```c++
// read image to file using Gdiplus
CodecGdiplus::SaveFile(L"d:\\a.jpg", img, 80);

// read image to file using WIC
auto   bmp = CWICFunc::CreateBitmapFromHBITMAP(img, WICBitmapUseAlpha);
CWICFileEncoder   writer(L"d:\\a.jpg", 80);
writer.WriteFile(bmp);
```
You might wonder why the WIC version requires 3-lines of code and an additional conversion. Let me explain: IWICBitmapFrameEncode::WritePixels can write memory data directly. However, when saving as a JPEG, it requires that the data source is a 24-bit color format. If we pass in 32-bit color data, it will save the colors incorrectly. Since a format conversion is unavoidable, let's leave it to WIC to do that.

Although Microsoft no longer updates GDI+, and WIC is indeed powerful enough, but GDI+ has a important feature that WIC lacks: creating a bitmap object from a provided memory address. Here's an example:

## Draw on the bitmap
```c++
// add this line -> using namespace Gdiplus;
Image   img;
img.Create(500, 300, 32);
img.ApplyEffect(effect::FillColor(Color::PapayaWhip));

// Here a GDI+ bitmap object is created.
// and it doesn't allocate memory but directly uses the memory we provide
auto   gpbmp = CodecGdiplus::CreateBitmapReference(img);
Graphics   gc(gpbmp.get());
gc.SetSmoothingMode(SmoothingModeAntiAlias);

// draw a rect on the bitmap using GDI+
Gdiplus::Font   font(L"Arial", (REAL)32);
SolidBrush   br(Color::DarkGreen);
gc.FillEllipse(&br, Rect(300, 100, 100, 100));
gc.DrawString(L"Welcome to PhoXo!", -1, &font, PointF(30, 20), &br);

// draw a line with arrow
GraphicsPath   gp;
PointF   pts[] = { {-2.5, -2.5}, {0, -0.5}, {2.5, -2.5}, {0, 2.5} };
gp.AddPolygon(pts, 4);
CustomLineCap   user_cap(&gp, NULL);
Pen   pen(Color::DarkGreen, 10);
pen.SetStartCap(LineCapRound);
pen.SetCustomEndCap(&user_cap);
gc.DrawLine(&pen, 50, 100, 200, 230);

CodecGdiplus::SaveFile(L"d:\\out.png", img);
```
This is the program's output:

![out](https://github.com/phoxo/UIStone/assets/168192359/40b19e8f-08e9-4477-9823-b4b5512b2b10)

As you can see, high-quality rendering in GDI+ can be applied directly to the bitmap. Many operations in PhoXo are done this way.

> **Draw Image**

Using GDI and GDI+, you can easily render our bitmaps to the screen. Please note that if you use the GDI method, 32-bit color bitmaps require pre-multiple alpha in advance.
```c++
// draw image using GDI
if ((img.ColorBits() == 32) && !img.IsPremultiplied())
{
    img.ApplyEffect(effect::Premultiply());
}

// using GDI+
Graphics   gc(hdc);
auto   gdip_bmp = CodecGdiplus::CreateBitmapReference(img);
gc.SetInterpolationMode(InterpolationModeHighQualityBicubic);
gc.DrawImage(gdip_bmp.get(), 0, 0, 200, 200);
```

## Image processing

One of the most important feature of ImageStone is the ability to handle images with ease. In addition to a wide range of built-in effects, you can also easily add new effects.
```c++
// This is a piece of code for applying Gaussian blur to an image
Image   img;
CodecWIC::LoadFile(L"d:\\1.jpg", img);

effect::StackBlur   cmd(20);
cmd.EnableParallel(true);
img.ApplyEffect(cmd);

CodecGdiplus::SaveFile(L"d:\\out.jpg", img);
```
Please note the **EnableParallel** switch. Enabling it allows the use of multithreading to maximize the utilization of modern multi-core CPUs. The processing time for a 5365x4194 image with the above code decreases from 950 milliseconds to 170 milliseconds, Most image processing algorithms experience a 5-10 times speed improvement.


