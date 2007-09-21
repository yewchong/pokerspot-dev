#ifndef __psi_banner_h__
#define __psi_banner_h__


// Support for scrolling banner text

class CBanner
{
public:
    CBanner(CWnd*, const CRect&);
    ~CBanner();

    void setBannerText(const CString&);
    void setBannerTextScroll(const CString&, int speed);

    void onDraw(CDC*);
    void tick(DWORD);

    void start();
    void stop();

    static void CALLBACK BannerProc(HWND, UINT, UINT, DWORD);

private:
    CWnd*   wnd_;
    CString text_;
    CPoint  pos_;
    CRect   rect_;
    CBitmap bmp_;
    int     timer_;
};


#endif