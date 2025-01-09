using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEditor;
using UnityEngine;

namespace ps2unity {
    public class LightmapExporter {
        public static bool ProcessExposure(int id, float fStops) {
            string lightMapsFolder = UnityUtil.GetLightmapsFolder();

            DirectoryInfo sceneDir = new DirectoryInfo(lightMapsFolder);
            FileInfo[] maps = sceneDir.GetFiles("*.exr");
            FileInfo first = maps.FirstOrDefault(c => c.Name.Contains("_comp_light"));
            if (first == null) {
                return false;
            }

            string lightMapFile = Path.Combine(lightMapsFolder, first.Name);
            Texture2D texture = AssetDatabase.LoadAssetAtPath<Texture2D>(lightMapFile);
            if (texture == null) {
                return false;
            }
            Material exposureMat = ExporterResources.ExposureMaterial;
            exposureMat.SetFloat("_RelFStops", fStops);
            exposureMat.SetFloat("_IsLinear", PlayerSettings.colorSpace == ColorSpace.Linear ? 1 : 0);
            exposureMat.SetTexture("_InputTex", texture);

            // We need to access unity_Lightmap_HDR to decode the lightmap,
            // but we can't, so we have to render everything to a custom RenderTexture!
            Texture2D decTex = ExporterResources.TempRenderTexture;
            decTex.name = "LightmapPreview";

            RenderTexture renderTexture = RenderTexture.GetTemporary(decTex.width, decTex.height);
            Graphics.SetRenderTarget(renderTexture);
            GL.Clear(true, true, new Color(0, 0, 0, 0)); // clear to transparent

            exposureMat.SetPass(0);
            Graphics.DrawMeshNow(ExporterResources.PlaneMesh, Matrix4x4.identity);

            decTex.ReadPixels(new Rect(0, 0, decTex.width, decTex.height), 0, 0);
            decTex.Apply();

            Graphics.SetRenderTarget(null);
            RenderTexture.ReleaseTemporary(renderTexture);
            return true;
        }
    }
}
