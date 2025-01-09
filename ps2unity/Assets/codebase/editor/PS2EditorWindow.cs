using System;
using System.Collections.Generic;
using System.IO;
using UnityEditor;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UIElements;
using Scene = UnityEngine.SceneManagement.Scene;

namespace ps2unity {
    public class PS2EditorWindow : EditorWindow {
        [MenuItem("Window/PS2 Editor")]
        public static void ShowExample() {
            PS2EditorWindow wnd = GetWindow<PS2EditorWindow>();
            wnd.titleContent = new GUIContent("PlayStation 2 Exporter");
        }

        public void CreateGUI() {
            // Each editor window contains a root VisualElement object
            VisualElement root = rootVisualElement;

            // Create button
            Button btnExport = new Button();
            btnExport.name = "export";
            btnExport.text = "Export Scene";
            btnExport.clicked += BtnExport_clicked;
            root.Add(btnExport);
        }

        private void WriteObjects(GameObject[] gos, BinaryWriter writer, BinaryWriter assetsWriter, Dictionary<object, string> assets) {
            writer.Write((uint)gos.Length);

            for (int i = 0; i < gos.Length; i++) {
                GameObject go = gos[i];
                WriteObjectsChild(go.transform, writer, assetsWriter, assets);
            }
        }

        private void SerializeMesh(Mesh mesh, BinaryWriter writer) {
            Vector3[] vertices = mesh.vertices;
            Vector3[] normals = mesh.normals;
            Vector2[] uv = mesh.uv;
            int[] indices = mesh.triangles;

            // mesh type: 120 (PS2, triangle list no index all float4)
            writer.Write((byte)120);

            // engine doesnt support indices, so as disk space is not a problem we save all triangles
            writer.Write((uint)indices.Length);

            for (int i = 0; i < indices.Length - 2; i += 3)
            {
                int i0 = indices[i];
                int i1 = indices[i + 1];
                int i2 = indices[i + 2];
                writer.Write(vertices[i0]);
                writer.Write(1.0f);
                writer.Write(vertices[i1]);
                writer.Write(1.0f);
                writer.Write(vertices[i2]);
                writer.Write(1.0f);
            }

            for (int i = 0; i < indices.Length - 2; i += 3)
            {
                int i0 = indices[i];
                int i1 = indices[i + 1];
                int i2 = indices[i + 2];
                writer.Write(normals[i0]);
                writer.Write(1.0f);
                writer.Write(normals[i1]);
                writer.Write(1.0f);
                writer.Write(normals[i2]);
                writer.Write(1.0f);
            }

            for (int i = 0; i < indices.Length - 2; i += 3)
            {
                int i0 = indices[i];
                int i1 = indices[i + 1];
                int i2 = indices[i + 2];
                writer.Write(uv[i0]);
                writer.Write(1.0f);
                writer.Write(1.0f);
                writer.Write(uv[i1]);
                writer.Write(1.0f);
                writer.Write(1.0f);
                writer.Write(uv[i2]);
                writer.Write(1.0f);
                writer.Write(1.0f);
            }
        }

        private void WriteObjectsChild(Transform go, BinaryWriter writer, BinaryWriter assetsWriter, Dictionary<object, string> assets) {
            writer.Write(go.transform.localPosition);
            writer.Write(go.transform.localScale);
            writer.Write(go.transform.localRotation);

            Component[] comps = go.GetComponents(typeof(Component));
            long compsPos = writer.BaseStream.Position;
            writer.Write((byte)0);

            int totalComps = comps.Length;
            for (int i = 0; i < comps.Length; i++) {
                Component comp = comps[i];

                if (comp is MeshRenderer) {
                    writer.Write((ushort)ComponentType.Mesh);

                    MeshRenderer meshRen = (MeshRenderer)comp;
                    MeshFilter meshFilter = comp.GetComponent<MeshFilter>();
                    Mesh sharedMesh = meshFilter.sharedMesh;

                    string key;
                    if (!assets.TryGetValue(sharedMesh, out key)) {
                        // serialize asset
                        key = GUID.Generate().ToString();
                        assets.Add(sharedMesh, key);

                        assetsWriter.Write(key.ToCharArray());
                        assetsWriter.Write((ushort)AssetType.Mesh);
                        SerializeMesh(sharedMesh, assetsWriter);
                    }

                    writer.Write(key.ToCharArray());
                } else {
                    totalComps--;
                    //writer.Write((ushort)ComponentType.None);
                    continue;
                }
            }

            long finalPos = writer.BaseStream.Position;
            writer.BaseStream.Position = compsPos;
            writer.Write((byte)totalComps);
            writer.BaseStream.Position = finalPos;

            int counter = 0;
            foreach (Transform tr in go.transform) {
                counter++;
            }

            writer.Write(counter);
            foreach (Transform tr in go.transform) {
                WriteObjectsChild(tr, writer, assetsWriter, assets);
            }
        }

        private void BtnExport_clicked() {
            Scene scene = SceneManager.GetActiveScene();

            using MemoryStream stream = new MemoryStream();
            using BinaryWriter writer = new BinaryWriter(stream);

            using MemoryStream streamAssets = new MemoryStream();
            using BinaryWriter writerAssets = new BinaryWriter(streamAssets);

            writer.Write(new char[] { 'N', 'U', 'K', 'E' });
            writer.Write((uint)0);
            writer.Write((uint)100);

            writerAssets.Write(new char[] { 'N', 'U', 'K', 'E' });
            writerAssets.Write((uint)1);
            writerAssets.Write((uint)100);

            GameObject[] gos = scene.GetRootGameObjects();
            Dictionary<object, string> assets = new Dictionary<object, string>();
            WriteObjects(gos, writer, writerAssets, assets);

            writer.Flush();
            writerAssets.Flush();

            string baseFolder = "D:\\dev\\ps2unity\\ps2engine\\ps2engine";
            string fileName = Path.Combine(baseFolder, "scene.ps2");
            string assetsName = Path.Combine(baseFolder, "assets.ps2");

            if (File.Exists(fileName)) {
                File.Delete(fileName);
            }
            if (File.Exists(assetsName)) {
                File.Delete(assetsName);
            }

            using (Stream fileStr = File.OpenWrite(fileName)) {
                stream.Position = 0;
                stream.CopyTo(fileStr);
            }

            using (Stream fileStr = File.OpenWrite(assetsName)) {
                streamAssets.Position = 0;
                streamAssets.CopyTo(fileStr);
            }
        }
    }
}