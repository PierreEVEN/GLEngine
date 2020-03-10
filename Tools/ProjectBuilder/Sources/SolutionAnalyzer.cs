using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace ProjectBuilder
{
    struct SolutionStruct
    {       
        public String Name { get; set; } //Solution name
        public String BinariesFolder { get; set; } //Compiled binaries path
        public String LibraryOutputFolder { get; set; } //Compiled library path
        public String ExecutableOutputFolder { get; set; } //Compiled executable output folder
        public String PlatformToolset { get; set; } //Plateform toolset version
        public String WindowTargetPlateformVersion { get; set; } //Windows plateform toolset version       
        public String ToolVersion { get; set; } //Tool version
    }

    struct ProjectStruct
    {
        public String ProjectName { get; set; } //Project name  
        public String IncludesPath { get; set; } //Project includes path     
        public String ProjectType { get; set; } //Project type
        public List<string> Dependencies { get; set; } //Project dependencies
        public List<string> AdditionalIncludes { get; set; } //Optional additional include paths
        public List<string> AdditionalLibraries { get; set; } //Optional additional library paths
        public String ProjectFileAbsolutePath { get; set; } //Project absolute path  
    }

    class SolutionAnalyzer
    {
        public List<ProjectStruct> Projects { get; private set; }
        public SolutionStruct solutionData { get; private set; }
        public static String SolutionAbsolutePath { get; private set; }
        public static String SolutionRelativePath { get; private set; }
        private static SolutionAnalyzer analyzerInstance { get; set; }

        public static void SetSolutionAbsolutePath(String inPath)
        {
            SolutionAbsolutePath = inPath;
            SolutionRelativePath = Path.GetRelativePath(".", inPath);
        }
        public static SolutionAnalyzer Get()
        {
            if (analyzerInstance == null)
            {
                analyzerInstance = new SolutionAnalyzer();
            }
            return analyzerInstance;
        }
        public static ProjectStruct GetProject(String inProjectName)
        {
            foreach(ProjectStruct project in SolutionAnalyzer.Get().Projects)
            {
                if (project.ProjectName == inProjectName) return project;
            }
            throw new FieldAccessException("Project " + inProjectName + " doesn't exist");
        }
        private SolutionAnalyzer()
        {
            GatherSolution();
            GatherProjects();
        }
        private void GatherSolution()
        {
            String solutionFile = "";
            try
            {
                solutionFile = ProjLibrary.GetFilesInDirectory(SolutionAbsolutePath, ".conf")[0];
            }
            catch
            {
                Console.WriteLine("Failed to find solution in '" + SolutionAbsolutePath + "' folder (verify .conf files)");
            }

            try
            {
                StreamReader fs = new StreamReader(solutionFile);
                solutionData = JsonConvert.DeserializeObject<SolutionStruct>(fs.ReadToEnd());

                Console.WriteLine("#################################### Begin generation for " + solutionData.Name + " ####################################");
            }
            catch
            {
                Console.WriteLine("Failed to parse solution config path : " + solutionFile);
            }
        }
        private void GatherProjects()
        {
            List<String> projectsFiles = ProjLibrary.GetFilesInDirectory(SolutionAbsolutePath, ".proj");

            Projects = new List<ProjectStruct>();

            foreach (String project in projectsFiles)
            {
                try
                {
                    StreamReader fs = new StreamReader(project);
                    ProjectStruct projectData = JsonConvert.DeserializeObject<ProjectStruct>(fs.ReadToEnd());

                    foreach (ProjectStruct Project in Projects)
                    {
                        if (Project.ProjectName == projectData.ProjectName) throw new InvalidDataException("Duplicated project name : " + projectData.ProjectName);
                    }
                    projectData.ProjectFileAbsolutePath = Path.GetFullPath(project);
                    Projects.Add(projectData);
                    Console.WriteLine(" --- Importing project '" + projectData.ProjectName + "'");
                }
                catch (Exception error)
                {
                    Console.WriteLine("Failed to parse project " + project + " :\n" + error);
                }
            }
        }

        public static String StackStringList(List<String> inList)
        {
            String outString = "";
            foreach (String elem in inList)
            {
                outString += elem + ";";
            }
            return outString;
        }
        public List<String> GetIncludes(String ProjectName)
        {
            List<String> outIncludes = new List<String>();
            ProjectStruct projData = GetProject(ProjectName);
            outIncludes.Add(Path.GetDirectoryName(projData.ProjectFileAbsolutePath) + "\\" + projData.IncludesPath);
            if (projData.Dependencies != null)
            {
                foreach (String dependency in projData.Dependencies)
                {
                    foreach (String subInclude in GetIncludes(dependency))
                    {
                        outIncludes.Add(subInclude);
                    }
                }
            }
            if (projData.AdditionalIncludes != null)
            {
                foreach (String include in projData.AdditionalIncludes)
                {
                    outIncludes.Add(SolutionAbsolutePath + "\\" + include);
                }
            }
            return outIncludes;
        }

        public List<String> GetLibraries(String projectName, String configuration, String plateform)
        {
            List<String> outIncludes = new List<String>();
            ProjectStruct projData = GetProject(projectName);
            if (projData.Dependencies != null)
            {
                foreach (String dependency in projData.Dependencies)
                {
                    ProjectStruct SubProject = GetProject(dependency);
                    if (SubProject.ProjectType == "StaticLibrary")
                    {
                        outIncludes.Add(SolutionAbsolutePath + "\\" + solutionData.LibraryOutputFolder + "\\" + configuration + "\\" + plateform + "\\" + SubProject.ProjectName + "\\" + SubProject.ProjectName + ".lib");

                        foreach (String subLibrary in GetLibraries(SubProject.ProjectName, configuration, plateform))
                        {
                            outIncludes.Add(subLibrary);
                        }
                    }
                }
            }
            if (projData.AdditionalLibraries != null)
            {
                foreach (String dependency in projData.AdditionalLibraries)
                {
                    outIncludes.Add(SolutionAbsolutePath + "\\" + dependency);
                }
            }
            return outIncludes;
        }
    }
}
