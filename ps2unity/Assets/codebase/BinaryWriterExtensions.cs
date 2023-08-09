using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace ps2unity {
    public static class BinaryWriterExtensions {
        public static void Write(this BinaryWriter writer, Vector2 value) {
            writer.Write(value.x);
            writer.Write(value.y);
        }

        public static void Write(this BinaryWriter writer, Vector3 value) {
            writer.Write(value.x);
            writer.Write(value.y);
            writer.Write(value.z);
        }

        public static void Write(this BinaryWriter writer, Vector4 value) {
            writer.Write(value.x);
            writer.Write(value.y);
            writer.Write(value.z);
            writer.Write(value.w);
        }

        public static void Write(this BinaryWriter writer, Quaternion value) {
            writer.Write(value.x);
            writer.Write(value.y);
            writer.Write(value.z);
            writer.Write(value.w);
        }
    }
}
