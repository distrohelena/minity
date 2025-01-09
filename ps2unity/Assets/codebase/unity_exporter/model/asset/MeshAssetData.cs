using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace ps2unity {
    public class MeshAssetData : AssetData {
        public List<Vector3> vertices;
        public List<Vector3> normals;
        public List<Vector2> uv;
        public List<Vector2> uv1;
        public List<int> indices;
    }
}
