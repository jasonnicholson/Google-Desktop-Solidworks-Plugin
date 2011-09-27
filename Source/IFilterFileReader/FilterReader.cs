using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Runtime.InteropServices.ComTypes;

namespace IFilterFileReader
{
  /// <summary>
  /// Implements a TextReader that reads from an IFilter. 
  /// </summary>
  public class FilterReader : TextReader
  {
    IFilter _filter;
    private bool _done;
    private STAT_CHUNK _currentChunk;
    private bool _currentChunkValid;
    private char[] _charsLeftFromLastRead;
      
    public override void Close()
    {
      Dispose(true);
      GC.SuppressFinalize(this);
    }

    ~FilterReader()
    {
      Dispose(false);
    }

    protected override void Dispose(bool disposing)
    {
        if (_filter != null)
        {
            Marshal.ReleaseComObject(_filter);
            _filter = null;
        }
    }
      
    public override int Read(char[] array, int offset, int count)
    {
      int endOfChunksCount=0;
      int charsRead=0;
      bool valChunkNeedsNewline = true;

      while (!_done && charsRead<count)
      {
        if (_charsLeftFromLastRead!=null)
        {
          int charsToCopy=(_charsLeftFromLastRead.Length<count-charsRead)?_charsLeftFromLastRead.Length:count-charsRead;
          Array.Copy(_charsLeftFromLastRead,0, array, offset+charsRead, charsToCopy);
          charsRead+=charsToCopy;
          if (charsToCopy<_charsLeftFromLastRead.Length)
          {
            char[] tmp=new char[_charsLeftFromLastRead.Length-charsToCopy];
            Array.Copy(_charsLeftFromLastRead, charsToCopy, tmp, 0, tmp.Length);
            _charsLeftFromLastRead=tmp;
          }
          else
            _charsLeftFromLastRead=null;
          continue;
        };

        if (!_currentChunkValid)
        {
          IFilterReturnCode res=_filter.GetChunk(out _currentChunk);
          _currentChunkValid = (res == IFilterReturnCode.S_OK) && (((_currentChunk.flags & CHUNKSTATE.CHUNK_TEXT) | (_currentChunk.flags & CHUNKSTATE.CHUNK_VALUE)) != 0);

          if (res==IFilterReturnCode.FILTER_E_END_OF_CHUNKS)
            endOfChunksCount++;

          if (endOfChunksCount>1)
            _done=true; //That's it. no more chuncks available
        }

        if (_currentChunkValid)
        {
            if ((_currentChunk.flags & CHUNKSTATE.CHUNK_TEXT) != 0)
            {
                uint bufLength = (uint)(count - charsRead);
                if (bufLength < 8192)
                    bufLength = 8192; //Read ahead

                char[] buffer = new char[bufLength];
                IFilterReturnCode res = _filter.GetText(ref bufLength, buffer);
                if (res == IFilterReturnCode.S_OK || res == IFilterReturnCode.FILTER_S_LAST_TEXT)
                {
                    int cRead = (int)bufLength;
                    if (cRead + charsRead > count)
                    {
                        int charsLeft = (cRead + charsRead - count);
                        _charsLeftFromLastRead = new char[charsLeft];
                        Array.Copy(buffer, cRead - charsLeft, _charsLeftFromLastRead, 0, charsLeft);
                        cRead -= charsLeft;
                    }
                    else
                        _charsLeftFromLastRead = null;

                    Array.Copy(buffer, 0, array, offset + charsRead, cRead);
                    charsRead += cRead;
                }
                if (res == IFilterReturnCode.FILTER_S_LAST_TEXT || res == IFilterReturnCode.FILTER_E_NO_MORE_TEXT)
                    _currentChunkValid = false;
            }
            else if ((_currentChunk.flags & CHUNKSTATE.CHUNK_VALUE) != 0)
            {
                valChunkNeedsNewline = !valChunkNeedsNewline;
                PropVariant pValue = new PropVariant();

                // http://blogs.msdn.com/dimeby8/archive/2006/12/11/wpd-property-retrieval-in-c.aspx
                // To convert from our C# PropVariant to the interop IntPtr:
                IntPtr ptrValue = Marshal.AllocCoTaskMem(Marshal.SizeOf(pValue));
                Marshal.StructureToPtr(pValue, ptrValue, false);
                
                IFilterReturnCode res = (IFilterReturnCode)_filter.GetValue(ref ptrValue);

                PropVariant pv = (PropVariant)Marshal.PtrToStructure(ptrValue, typeof(PropVariant));

                // for PropId defenition: http://addins.msn.com/devguide.aspx
                if (res == IFilterReturnCode.S_OK)
                {
                    String strVal = pv.Value.ToString() + "\n";
                    char[] valChars = strVal.ToCharArray();
                    int valCharsLen = valChars.Length;

                    int cRead = valCharsLen;
                    if (cRead + charsRead > count)
                    {
                        int charsLeft = (cRead + charsRead - count);
                        _charsLeftFromLastRead = new char[charsLeft];
                        Array.Copy(valChars, cRead - charsLeft, _charsLeftFromLastRead, 0, charsLeft);
                        cRead -= charsLeft;
                    }
                    else
                        _charsLeftFromLastRead = null;

                    Array.Copy(valChars, 0, array, offset + charsRead, cRead);
                    charsRead += cRead;
                }
                if (res == IFilterReturnCode.FILTER_S_LAST_VALUES || res == IFilterReturnCode.FILTER_E_NO_MORE_VALUES)
                    _currentChunkValid = false;

                pv.Clear();

                Marshal.FreeCoTaskMem(ptrValue);
                ptrValue = IntPtr.Zero;
            }
          }
      }
      return charsRead;
    }

    public FilterReader(string fileName)
    {
      _filter=FilterLoader.LoadAndInitIFilter(fileName);
      if (_filter==null)
        throw new ArgumentException("no filter defined for "+fileName);
    }
  }
}
