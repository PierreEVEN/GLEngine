using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace ProjectBuilder
{
    class filtersFileGenerator
    {

        private String SourceFolder;
        private String ProjectName;
        private Dictionary<String, String> Paths;


        public filtersFileGenerator(String inProjectName, String inSourceFodler)
        {
            ProjectName = inProjectName;
            SourceFolder = inSourceFodler;
            GeneratePaths(SourceFolder);
        }

        public void Build()
        {
            String outFile = "";

            outFile += "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
            outFile += "<Project ToolsVersion=\"4.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";

            GenerateSourceFilePaths(ref outFile);
            GenerateIdentifiers(ref outFile);
            outFile += "</Project>\n";

            String newFilePath = SourceFolder + "/" + ProjectName + ".vcxproj.filters";

            if (File.Exists(newFilePath)) File.Delete(newFilePath);

            FileStream stream = new FileStream(newFilePath, FileMode.CreateNew, FileAccess.Write);
            foreach (byte elem in outFile)
            {
                stream.WriteByte(elem);
            }
            stream.Close();

        }

        private void GeneratePaths(String basePath)
        {
            List<String> dirs = ProjLibrary.GetSubfoldersInDirectory(basePath);
            foreach (String dir in dirs)
            {
                TryAddPath(dir);
            }
        }
        private void TryAddPath(String inPath)
        {
            if (Path.GetRelativePath(SourceFolder, inPath) == ".") return;
            if (Paths == null) Paths = new Dictionary<String, String>();
            if (!Paths.ContainsKey(inPath))
            {
                Paths.Add(inPath, Path.GetRelativePath(SourceFolder, inPath));
            }
        }

        private void GenerateSourceFilePaths(ref String sourceFile)
        {
            sourceFile += "  <ItemGroup>\n";
            if (Paths == null) return;
            foreach(KeyValuePair<String, String> dir in Paths)
            {
                String[] subFiles = Directory.GetFiles(dir.Key);
                foreach (String file in subFiles)
                {
                    if (Path.GetExtension(file) == ".cpp" || Path.GetExtension(file) == ".c")
                    {
                        sourceFile += "    <ClCompile Include=\"" + Path.GetRelativePath(SourceFolder,file) + "\">\n";
                        sourceFile += "      <Filter>" + dir.Value + "</Filter>\n";
                        sourceFile += "    </ClCompile>\n";
                    }
                }
            }
            sourceFile += "  </ItemGroup>\n";
            sourceFile += "  <ItemGroup>\n";
            foreach (KeyValuePair<String, String> dir in Paths)
            {
                String[] subFiles = Directory.GetFiles(dir.Key);
                foreach (String file in subFiles)
                {
                    if (Path.GetExtension(file) == ".h")
                    {
                        sourceFile += "    <ClInclude Include=\"" + Path.GetRelativePath(SourceFolder, file) + "\">\n";
                        sourceFile += "      <Filter>" + dir.Value + "</Filter>\n";
                        sourceFile += "    </ClInclude>\n";
                    }
                }
            }
            sourceFile += "  </ItemGroup>\n";
        }
        private void GenerateIdentifiers(ref String sourceFile)
        {
            sourceFile += "  <ItemGroup>\n";
            foreach (KeyValuePair<String, String> dir in Paths)
            {
                sourceFile += "    <Filter Include=\"" + dir.Value + "\">\n";
                sourceFile += "      <UniqueIdentifier>{" + dir.Value + "}</UniqueIdentifier>\n";
                sourceFile += "    </Filter>\n";
            }
            sourceFile += "  </ItemGroup>\n";
        }
    }
}
