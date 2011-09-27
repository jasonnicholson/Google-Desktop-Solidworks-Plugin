using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;

namespace IFilterFileReader
{
    [Guid("436b5c60-cc59-11e0-9572-0800200c9a66")]
    public interface IManagedInterface
    {
        void loadFile(string fileName);
        string readFile();
    }

    [Guid("627c16d0-cc59-11e0-9572-0800200c9a66")]
    public class IFilterFileReader : IManagedInterface, IDisposable
    {
        FilterReader filterReader;

        public IFilterFileReader()
        {

        }

        public void Dispose()
        {
            if (filterReader != null)
            {
                filterReader.Dispose();
                filterReader = null;
            }
        }

        public IFilterFileReader(string fileName)
        {
            loadFile(fileName);
        }

        public void loadFile(string fileName)
        {
            filterReader = new FilterReader(fileName);
        }

        public string readFile()
        {
            return filterReader.ReadToEnd();
        }
    }
}
