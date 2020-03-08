using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace ProjectBuilder
{
    class ProjectBuilder
    {

        public static void WriteProjectFile(String FileValue, String FilePath)
        {
            if (File.Exists(FilePath)) File.Delete(FilePath);
            FileStream stream = new FileStream(FilePath, FileMode.CreateNew, FileAccess.Write);
            foreach (byte elem in FileValue)
            {
                stream.WriteByte(elem);
            }
            stream.Close();
        }
    }
}
