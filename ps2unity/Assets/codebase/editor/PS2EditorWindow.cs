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
            writer.Write(gos.Length);

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

            writer.Write((ushort)vertices.Length);
            writer.Write((ushort)normals.Length);
            writer.Write((ushort)uv.Length);
            writer.Write((ushort)indices.Length);

            for (int i = 0; i < vertices.Length; i++) {
                writer.Write(vertices[i]);
            }
            for (int i = 0; i < normals.Length; i++) {
                writer.Write(normals[i]);
            }
            for (int i = 0; i < uv.Length; i++) {
                writer.Write(uv[i]);
            }
            for (int i = 0; i < indices.Length; i++) {
                writer.Write((ushort)indices[i]);
            }
        }

        private void WriteObjectsChild(Transform go, BinaryWriter writer, BinaryWriter assetsWriter, Dictionary<object, string> assets) {
            writer.Write(go.transform.localPosition);
            writer.Write(go.transform.localScale);
            writer.Write(go.transform.localRotation);

            Component[] comps = go.GetComponents(typeof(Component));
            writer.Write((byte)comps.Length);

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

                        assetsWriter.Write(key);
                        SerializeMesh(sharedMesh, assetsWriter);
                    }

                    writer.Write(key);
                } else {
                    writer.Write((ushort)ComponentType.None);
                    continue;
                }
            }

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

            string fileName = "C:\\dev\\ps2\\scene.ps2";
            string assetsName = "C:\\dev\\ps2\\assets.ps2";

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