using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace ps2unity {
    public static class UnityUtil {
        //public static bool IsProceduralSkybox() {
        //    Material skybox = RenderSettings.skybox;
        //    if (skybox != null) {
        //        string[] skyboxTexNames = JanusGlobals.SkyboxTexNames;
        //        for (int i = 0; i < skyboxTexNames.Length; i++) {
        //            if (!skybox.HasProperty(skyboxTexNames[i])) {
        //                return true;
        //            }
        //        }
        //    }
        //    return false;
        //}

        public static IEnumerable<GameObject> GetSceneRoots2() {
            var prop = new HierarchyProperty(HierarchyType.GameObjects);
            var expanded = new int[0];
            while (prop.Next(expanded)) {
                yield return prop.pptrValue as GameObject;
            }
        }

        public static string GetLightmapsFolder() {
            string scenePath = Path.GetDirectoryName(GetScenePath());
            return Path.Combine(scenePath, GetSceneName());
        }

        public static string GetScenePath() {
#if UNITY_5_3_OR_NEWER
            return SceneManager.GetActiveScene().path;
#else
            return EditorApplication.currentScene;
#endif
        }

        public static string GetSceneName() {
#if UNITY_5_3_OR_NEWER
            return SceneManager.GetActiveScene().name;
#else
            return Path.GetFileNameWithoutExtension(EditorApplication.currentScene);
#endif
        }

        public static GameObject[] GetSceneRoots() {
#if UNITY_5_3_OR_NEWER
            Scene scene = SceneManager.GetActiveScene();
            return scene.GetRootGameObjects();
#else
            return GetSceneRoots2().ToArray();
#endif
        }

        public static void SetWindowTitle(this EditorWindow window, string title, Texture2D icon) {
#if UNITY_5_0
            window.title = "Janus";
#else
            window.titleContent = new GUIContent("Janus", icon);
#endif
        }

        public static void StartProcess(string exportPath) {
#if UNITY_5_3
            Process p = new Process();
            p.StartInfo.FileName = exportPath;
            p.Start();
#else
            System.Diagnostics.Process.Start(exportPath);
#endif
        }

        public static void WriteAllText(string exportPath, string text) {
#if UNITY_5_3
            using (Stream stream = File.OpenWrite(exportPath))
            {
                using (StreamWriter writer = new StreamWriter(stream))
                {
                    writer.Write(text);
                    writer.Flush();
                }
                stream.Flush();
            }
#else
            File.WriteAllText(exportPath, text);
#endif
        }
    }
}