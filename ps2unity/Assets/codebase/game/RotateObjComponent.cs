using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace ps2unity {
    public class RotateObjComponent : MonoBehaviour {
        public float speed = 2;

        private void Update() {
            float dt = Time.deltaTime;

            Vector3 euler = transform.localEulerAngles;
            euler.y += speed * dt;
            transform.localEulerAngles = euler;
        }
    }
}
