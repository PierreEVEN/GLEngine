using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace ProjectBuilder
{
    class ProjLibrary
    {
        static String currentFileString = "";
        static int currentTabIndex = 0;

        private static String GenerateXmlTab()
        {
            String result = "";
            for (int i = 0; i < currentTabIndex * 2; ++i)
            {
                result += ' ';
            }
            return result;
        }
        public static List<String> GetFilesInDirectory(String inDirectory, String inExtension)
        {
            List<String> dirs = new List<String>();
            if (!Directory.Exists(inDirectory)) throw new FileNotFoundException("Failed to find directory " + inDirectory);
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
        public static List<String> GetSubfoldersInDirectory(String inDirectory)
        {
            List<String> dirs = new List<String>();
            dirs.Add(inDirectory);
            foreach (String path in Directory.GetDirectories(inDirectory))
            {
                foreach (String dir in GetSubfoldersInDirectory(path))
                {
                    if (dir != ".") dirs.Add(dir);
                }
            }

            return dirs;
        }
        public static void BeginXmlCategory(String inCategory, String inMeta = "")
        {
            currentFileString += GenerateXmlTab() + "<" + inCategory + " " + (inMeta.Length > 0 ? inMeta : "") + ">\n";
            currentTabIndex += 1;
        }
        public static void EndXmlCategory(String inCategory)
        {
            currentTabIndex -= 1;
            currentFileString += GenerateXmlTab() + "</" + inCategory + ">\n";
        }
        public static void AddXmlValue(String inCategoryName, String inValue, String Meta = "")
        {
            currentFileString += GenerateXmlTab() + "<" + inCategoryName + ((Meta != "") ? " " + Meta : "") + ">" + inValue + "</" + inCategoryName + ">\n";
        }
        public static void BeginXmlEdition()
        {
            currentTabIndex = 0;
            currentFileString = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
        }
        public static String GetXmlString()
        {
            return currentFileString;
        }
    }
}
