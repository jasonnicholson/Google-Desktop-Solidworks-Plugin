// Copyright 2005-2007 Google Inc. All Rights Reserved.
//
// utils.cpp : Implementation of common utility functions.
//
#include "stdafx.h"
#include "basic_types.h"
#include "utils.h"

/**
* A minimalistic implementation of IStream on a buffer that is owned by 
* the user of the class.  It is the responsibility of the user of this class
* to make sure that the data remains valid as long as the StreamBuffer instance
* is being used.
*/
class StreamBuffer :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IStream {
 public:
  StreamBuffer() : 
    buffer_(NULL), 
    pos_(NULL), 
    end_(NULL) {
  }

BEGIN_COM_MAP(StreamBuffer)
  COM_INTERFACE_ENTRY(ISequentialStream)
  COM_INTERFACE_ENTRY(IStream)
END_COM_MAP()

  //
  //@name ISequentialStream
  //@{
  STDMETHOD(Read)(void* pv, ULONG cb, ULONG* read) {
    ATLASSERT(pv != NULL);
    ATLASSERT(cb != 0);
    // read may be NULL

    ATLASSERT(buffer_ != NULL);
    ATLASSERT(pos_ != NULL);
    ATLASSERT(end_ != NULL);

    HRESULT hr = S_OK;

    if (pos_ < end_) {
      ULONG can_read = min(cb, static_cast<ULONG>(end_ - pos_));

      memcpy(pv, pos_, can_read);
      pos_ += can_read;

      if (read)
        *read = can_read;
    } else if (read) {
      *read = 0;
      hr = S_FALSE;
    }

    return hr;
  }

  STDMETHOD(Write)(const void* pv, ULONG cb, ULONG* written) {
    return E_NOTIMPL;
  }

  //@}

  //
  //@name IStream
  //@{
  STDMETHOD(Seek)(LARGE_INTEGER move, ULONG origin, ULARGE_INTEGER* new_position) {
    // new_position may be NULL
    ATLASSERT(buffer_ != NULL);
    ATLASSERT(pos_ != NULL);
    ATLASSERT(end_ != NULL);

    const char* pos = NULL;
    switch (origin) {
      case STREAM_SEEK_SET:
        pos = buffer_ + move.QuadPart;
        break;

      case STREAM_SEEK_CUR:
        pos = pos_ + move.QuadPart;
        break;

      case STREAM_SEEK_END:
        pos = end_ + move.QuadPart;
        break;
    }

    if (pos == NULL || pos > end_ || pos < buffer_)
      return E_INVALIDARG;

    pos_ = pos;

    if (new_position)
      new_position->QuadPart = (pos_ - buffer_);

    return S_OK;
  }

  STDMETHOD(SetSize)(ULARGE_INTEGER libNewSize) {
    return E_NOTIMPL;
  }

  STDMETHOD(CopyTo)(IStream* stream, ULARGE_INTEGER cb, ULARGE_INTEGER* read, 
                    ULARGE_INTEGER* written) {
    ATLASSERT(stream != NULL);
    ATLASSERT(buffer_ != NULL);
    ATLASSERT(pos_ != NULL);
    ATLASSERT(end_ != NULL);
    if (!stream)
      return E_POINTER;

    if ((end_ - pos_) < cb.QuadPart)
      cb.QuadPart = (end_ - pos_);

    DWORD bytes_written = 0;
    HRESULT hr = stream->Write(pos_, cb.LowPart, &bytes_written);

    if (SUCCEEDED(hr)) {
      pos_ += bytes_written;

      if (written)
        written->QuadPart = bytes_written;

      if (read)
        read->QuadPart = bytes_written;
    }

    return hr;
  }

  STDMETHOD(Commit)(ULONG grfCommitFlags) {
    return E_NOTIMPL;
  }

  STDMETHOD(Revert)() {
    return E_NOTIMPL;
  }

  STDMETHOD(LockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, ULONG dwLockType) {
    return E_NOTIMPL;
  }

  STDMETHOD(UnlockRegion)(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, ULONG dwLockType) {
    return E_NOTIMPL;
  }

  STDMETHOD(Stat)(STATSTG *pstatstg, ULONG grfStatFlag) {
    ATLASSERT(pstatstg != NULL);
    ATLASSERT(buffer_ != NULL);
    ATLASSERT(pos_ != NULL);
    ATLASSERT(end_ != NULL);
    if (pstatstg == NULL)
      return E_POINTER;

    ZeroMemory(pstatstg, sizeof(STATSTG));
    if ((grfStatFlag & STATFLAG_NONAME) == 0) {
      const wchar_t *kStreamBuffer = L"StreamBuffer";
      int buffer_len = sizeof(kStreamBuffer);
      int wchar_len = buffer_len / sizeof(WCHAR);
      pstatstg->pwcsName = reinterpret_cast<wchar_t*>(
        ::CoTaskMemAlloc(buffer_len));
      if (pstatstg->pwcsName)
        lstrcpynW(pstatstg->pwcsName, kStreamBuffer, wchar_len);
    }

    pstatstg->cbSize.QuadPart = (end_ - buffer_);

    return S_OK;
  }

  STDMETHOD(Clone)(IStream **ppstm) {
    return E_NOTIMPL;
  }

  //@}

 public:
  /**
  * Routine to initialize set the internal buffers.
  * @param buffer A pointer to the data buffer. Must not be NULL.
  * @param size the size of the buffer.
  */
  void Initialize(const char* buffer, uint32 size) {
    ATLASSERT(buffer != NULL);
    ATLASSERT(buffer_ == NULL);
    ATLASSERT(pos_ == NULL);
    ATLASSERT(end_ == NULL);

    buffer_ = buffer;
    pos_ = buffer;
    end_ = (buffer + size);
  }

 protected:
  const char* buffer_;
  const char* pos_;
  const char* end_;
};

HRESULT LoadPictureFromResource(HMODULE module,
                                const TCHAR * res_name, const TCHAR * res_type,
                                IPicture **pict) {
  ATLASSERT(res_type);
  ATLASSERT(res_name);
  ATLASSERT(pict != NULL);
  HRSRC res_find_handle = FindResource(module, res_name, res_type);
  if (!res_find_handle)
    return E_INVALIDARG;

  HGLOBAL res_handle = LoadResource(module, res_find_handle);
  if (!res_handle)
    return E_INVALIDARG;

  DWORD data_size = SizeofResource(module, res_find_handle);
  char *src_ptr = reinterpret_cast<char *> (LockResource(res_handle));
  if (!src_ptr)
    return E_FAIL;

  CComObjectStackEx<StreamBuffer> stream;
  stream.Initialize(src_ptr, data_size);

  HRESULT hr = OleLoadPicture(&stream, data_size,
    FALSE, IID_IPicture, reinterpret_cast<void**>(pict));

  FreeResource(res_handle);
  return hr;  
}

void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr) {
  SetBkColor(hdc, clr);
  RECT rect = {x, y, x + cx, y + cy };
  ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
} 
