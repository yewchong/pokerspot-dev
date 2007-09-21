#ifndef __psi_maildoc_h__
#define __psi_maildoc_h__

// The purpose of this CDocument subclass is to provide the
// application with email support

class CMailDocument
{
public:
  CMailDocument(const CString& addr, const CString& body = CString());
  virtual ~CMailDocument();

  void setBody(const CString&);
  void setSubject(const CString&);
  void setAddress(const CString&);

  static BOOL IsMAPIAvailable();
         BOOL sendMail(bool showUI = true);

private:
  CString address_, subject_, body_;
};

#endif