using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ps2unity {
    public static class DirectoryUtil {
        public static void GetFiles(DirectoryInfo baseDir, Func<FileInfo, bool> filter, List<FileInfo> allFiles) {
            FileInfo[] files = baseDir.GetFiles();

            if (filter == null) {
                allFiles.AddRange(files);
            } else {
                for (int i = 0; i < files.Length; i++) {
                    FileInfo file = files[i];
                    if (filter(file)) {
                        continue;
                    }

                    allFiles.Add(file);
                }
            }

            DirectoryInfo[] childDirs = baseDir.GetDirectories();
            for (int i = 0; i < childDirs.Length; i++) {
                DirectoryInfo dirInfo = childDirs[i];
                GetFiles(dirInfo, filter, allFiles);
            }
        }

        public static List<FileInfo> GetFiles(DirectoryInfo baseDir, Func<FileInfo, bool> filter) {
            List<FileInfo> allFiles = new List<FileInfo>();
            GetFiles(baseDir, filter, allFiles);
            return allFiles;
        }

        public static List<FileInfo> GetFiles(DirectoryInfo baseDir) {
            List<FileInfo> allFiles = new List<FileInfo>();
            GetFiles(baseDir, null, allFiles);
            return allFiles;
        }
    }
}
