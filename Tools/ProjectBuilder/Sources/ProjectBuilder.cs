using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace ProjectBuilder
{
    struct SolutionStruct
    {
        public String Name { get; set; }
        public String BinariesFolder { get; set; }
        public String LibsFolder { get; set; }
        public String OutputFolder { get; set; }
        public String PlatformToolset { get; set; }
        public String WindowTargetPlateformVersion { get; set; }
        public String ToolVersion { get; set; }
    }

    struct ProjectStruct
    {
        public String ProjectName { get; set; }
        public String IncludesPath { get; set; }
        public String ProjectType { get; set; }
        public List<string> Dependencies { get; set; }
        public List<string> Libraries { get; set; }
        public List<string> AdditionalIncludes { get; set; }
        public String Projectpath { get; set; }
    }


    class ProjectBuilder
    {
        private String SolutionPath;
        private List<ProjectStruct> Projects;
        private List<String> LibPaths;
        private SolutionStruct solutionData;

        public ProjectBuilder(String inSolutionPath)
        {
            SolutionPath = inSolutionPath;
            try
            {
                List<String> files = ProjLibrary.GetFilesInDirectory(SolutionPath, ".conf");
                StreamReader fs = new StreamReader(files[0]);
                solutionData = JsonConvert.DeserializeObject<SolutionStruct>(fs.ReadToEnd());
                Projects = GetProjs();
            }
            catch (Exception e)
            {
                Console.WriteLine("Failed to read solution ");
                Console.WriteLine(e.Message);
            }
        }

        public void Build()
        {
            foreach (ProjectStruct elem in Projects)
            {

                List<String> Includes = BuildIncludes(elem.ProjectName);
                List<String> LibraryFolders = BuildLibs(elem.ProjectName);
                List<String> LibraryNames = new List<String>();
                foreach (String lib in LibraryFolders)
                {
                    foreach (String file in ProjLibrary.GetFilesInDirectory(Path.GetRelativePath(".", SolutionPath + "../Libs"), ".lib"))
                    {
                        LibraryNames.Add(Path.GetRelativePath(SolutionPath + "../Libs", file));
                    }
                }

                String currentPath = "." + Path.GetRelativePath(elem.Projectpath, SolutionPath) + elem.ProjectName;

                /** Build VCXPROJ File */
                vcxprojFileStringGenerator vcxProjGenerator = new vcxprojFileStringGenerator(elem.ProjectName, currentPath);
                vcxProjGenerator.bIsStaticLibrary = IsStaticLibrary(elem.ProjectType);
                vcxProjGenerator.libConfig = new List<LibConfig>();
                vcxProjGenerator.PlatformToolset = solutionData.PlatformToolset;
                vcxProjGenerator.WindowTargetPlateformVersion = solutionData.WindowTargetPlateformVersion;
                vcxProjGenerator.ToolVersion = solutionData.ToolVersion;
                vcxProjGenerator.outputFolder = IsStaticLibrary(elem.ProjectType) ? solutionData.LibsFolder : solutionData.OutputFolder;

                /** Build FILTERS file */
                filtersFileGenerator filtersGenerator = new filtersFileGenerator(elem.ProjectName, currentPath);

                LibConfig Win32DebugConfig = new LibConfig();
                Win32DebugConfig.Configuration = "Debug";
                Win32DebugConfig.Plateform = "Win32";
                Win32DebugConfig.LibsFolder = LibraryFolders;
                Win32DebugConfig.LibNames = LibraryNames;
                Win32DebugConfig.IncludeFolder = Includes;

                vcxProjGenerator.libConfig.Add(Win32DebugConfig);


                Console.WriteLine("Build module " + elem.ProjectName + " : ");
                Console.WriteLine("\t-ProjectType : " + elem.ProjectType);
                Console.WriteLine("\t" + "-includes");
                foreach (String include in Includes)
                {
                    Console.WriteLine("\t\t=>" + include);
                }
                Console.WriteLine("");
                Console.WriteLine("\t" + "-Libraries");
                foreach (String include in LibraryFolders)
                {
                    Console.WriteLine("\t\t=>" + include);
                }
                Console.WriteLine("");

                try
                {
                    vcxProjGenerator.Build();
                    filtersGenerator.Build();
                }
                catch(Exception e)
                {
                    Console.WriteLine(e.Message);
                }



                /** Build User proj file */
                if (!IsStaticLibrary(elem.ProjectType))
                {
                    String UserProj = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
                    UserProj += "<Project ToolsVersion=\"Current\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";
                    UserProj += "  <PropertyGroup Condition=\"'$(Configuration)' == 'Debug'\">\n";
                    UserProj += "    <LocalDebuggerWorkingDirectory>" + solutionData.OutputFolder + "</LocalDebuggerWorkingDirectory>\n";
                    UserProj += "    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>\n";
                    UserProj += "  </PropertyGroup>\n";
                    UserProj += "</Project>\n";

                    String newFilePath = currentPath + "/" + elem.ProjectName + ".vcxproj.user";

                    if (File.Exists(newFilePath)) File.Delete(newFilePath);

                    FileStream stream = new FileStream(newFilePath, FileMode.CreateNew, FileAccess.Write);
                    foreach (byte elemChar in UserProj)
                    {
                        stream.WriteByte(elemChar);
                    }
                    stream.Close();
                }



            }
        }

        private bool IsStaticLibrary(String projectType)
        {
            return projectType == "StaticLibrary";
        }

        private bool GetProj(String projName, out ProjectStruct result)
        {
            foreach(ProjectStruct proj in Projects)
            {
                if (proj.ProjectName == projName)
                {
                    result = proj;
                    return true;
                }
            }
            result = new ProjectStruct();
            return false;
        }

        private List<ProjectStruct> GetProjs()
        {
            List<String> files = ProjLibrary.GetFilesInDirectory(SolutionPath, ".proj");
            List<ProjectStruct> projects = new List<ProjectStruct>();

            foreach (String file in files)
            {
                try
                {
                    StreamReader fs = new StreamReader(file);
                    ProjectStruct Data = JsonConvert.DeserializeObject<ProjectStruct>(fs.ReadToEnd());
                    Data.Projectpath = Path.GetDirectoryName(Path.GetRelativePath(SolutionPath + "/../", file)) + "\\";
                    projects.Add(Data);
                }
                catch(Exception e)
                {
                    Console.WriteLine("Failed to read proj " + file);
                    Console.WriteLine(e.Message);
                }
            }

            return projects;
        }

        private List<String> BuildIncludes(String projectName)
        {
            List<String> includes = new List<string>();

            ProjectStruct project;
            GetProj(projectName, out project);

            includes.Add(project.Projectpath + project.IncludesPath); ;
            foreach (String proj in project.Dependencies)
            {
                ProjectStruct subProj;
                if (GetProj(proj, out subProj))
                {
                    includes.Add(subProj.Projectpath + subProj.IncludesPath);
                }
            }

            if (project.AdditionalIncludes != null)
            {
                foreach (String include in project.AdditionalIncludes)
                {
                    includes.Add(include);
                }
            }
            return includes;
        }


        private List<String> BuildLibs(String projectName)
        {
            ProjectStruct project;
            if (GetProj(projectName, out project))
            {
               return project.Libraries;
            }
            return null;
        }
    }
}
