using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace ProjectBuilder
{
    class filtersFileGenerator
    {
        public static String BuildFilterString(ProjectStruct inProject)
        {
            List<String> Paths = new List<String>();

            ProjLibrary.BeginXmlEdition();
            ProjLibrary.BeginXmlCategory("Project", "DefaultTargets=\"Build\" ToolsVersion=\"15.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\"");
            {
                ProjLibrary.BeginXmlCategory("ItemGroup");
                {
                    foreach (String file in ProjLibrary.GetFilesInDirectory(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath), ".h"))
                    {
                        String relativePath = Path.GetRelativePath(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath), file);
                        ProjLibrary.BeginXmlCategory("ClInclude", "Include=" + "\"" + relativePath + "\"");
                        {
                            AddUniquePath(Path.GetDirectoryName(relativePath), ref Paths);
                            ProjLibrary.AddXmlValue("Filter", Path.GetDirectoryName(relativePath));
                        }
                        ProjLibrary.EndXmlCategory("ClInclude");
                    }
                }
                ProjLibrary.EndXmlCategory("ItemGroup");
                ProjLibrary.BeginXmlCategory("ItemGroup");
                {
                    foreach (String file in ProjLibrary.GetFilesInDirectory(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath), ".cpp"))
                    {
                        String relativePath = Path.GetRelativePath(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath), file);
                        ProjLibrary.BeginXmlCategory("ClCompile", "Include=" + "\"" + relativePath + "\"");
                        {
                            AddUniquePath(Path.GetDirectoryName(relativePath), ref Paths);
                            ProjLibrary.AddXmlValue("Filter", Path.GetDirectoryName(relativePath));
                        }
                        ProjLibrary.EndXmlCategory("ClCompile");
                    }
                }
                ProjLibrary.EndXmlCategory("ItemGroup");
                ProjLibrary.BeginXmlCategory("ItemGroup");
                {
                    foreach (String file in ProjLibrary.GetFilesInDirectory(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath), ".c"))
                    {
                        String relativePath = Path.GetRelativePath(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath), file);
                        ProjLibrary.BeginXmlCategory("ClCompile", "Include=" + "\"" + relativePath + "\"");
                        {
                            AddUniquePath(Path.GetDirectoryName(relativePath), ref Paths);
                            ProjLibrary.AddXmlValue("Filter", Path.GetDirectoryName(relativePath));
                        }
                        ProjLibrary.EndXmlCategory("ClCompile");
                    }
                }
                ProjLibrary.EndXmlCategory("ItemGroup");

                foreach (String dir in ProjLibrary.GetSubfoldersInDirectory(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath)))
                {
                    AddUniquePath(Path.GetDirectoryName(Path.GetRelativePath(Path.GetDirectoryName(inProject.ProjectFileAbsolutePath), dir)), ref Paths);
                }

                ProjLibrary.BeginXmlCategory("ItemGroup");
                {
                    foreach (String path in Paths)
                    {
                        ProjLibrary.BeginXmlCategory("Filter", "Include=" + "\"" + path + "\"");
                        {
                            ProjLibrary.AddXmlValue("UniqueIdentifier", "{" + path + "}");
                        }
                        ProjLibrary.EndXmlCategory("Filter");

                    }
                }
                ProjLibrary.EndXmlCategory("ItemGroup");
            }
            ProjLibrary.EndXmlCategory("Project");

            return ProjLibrary.GetXmlString();
        }


        private static void AddUniquePath(String inPath, ref List<String> inList)
        {
            if (inList.Contains(inPath) || inPath == "" || inPath == ".") return;
            inList.Add(inPath);
        }



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
