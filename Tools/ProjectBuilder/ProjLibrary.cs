using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace ProjectBuilder
{
    class ProjLibrary
    {
        public static List<String> GetFilesInDirectory(String inDirectory, String inExtension)
        {
            List<String> dirs = new List<String>();
            foreach (String file in Directory.GetFiles(inDirectory))
            {
                if (Path.GetExtension(file) == inExtension)
                {
                    dirs.Add(file);
                }
            }
            foreach (String dir in Directory.GetDirectories(inDirectory))
            {
                foreach (String file in GetFilesInDirectory(dir, inExtension))
                {
                    if (Path.GetExtension(file) == inExtension)
                    {
                        dirs.Add(file);
                    }
                }
            }
            return dirs;
        }

    }
}
