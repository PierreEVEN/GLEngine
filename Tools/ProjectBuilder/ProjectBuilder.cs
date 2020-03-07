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
                List<String> Libraries = BuildLibs(elem.ProjectName);

                vcxprojFileStringGenerator vcxProjGenerator = new vcxprojFileStringGenerator(elem.ProjectName, "." + Path.GetRelativePath(elem.Projectpath, SolutionPath) + elem.ProjectName);
                vcxProjGenerator.bIsStaticLibrary = IsStaticLibrary(elem.ProjectType);
                vcxProjGenerator.libConfig = new List<LibConfig>();
                vcxProjGenerator.PlatformToolset = solutionData.PlatformToolset;
                vcxProjGenerator.WindowTargetPlateformVersion = solutionData.WindowTargetPlateformVersion;
                vcxProjGenerator.ToolVersion = solutionData.ToolVersion;

                LibConfig Win32DebugConfig = new LibConfig();
                Win32DebugConfig.Configuration = "Debug";
                Win32DebugConfig.Plateform = "Win32";
                Win32DebugConfig.LibNames = Libraries;
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
                foreach (String include in Libraries)
                {
                    Console.WriteLine("\t\t=>" + include);
                }
                Console.WriteLine("");


                try
                {
                    vcxProjGenerator.Build();
                }
                catch(Exception e)
                {
                    Console.WriteLine(e.Message);
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
