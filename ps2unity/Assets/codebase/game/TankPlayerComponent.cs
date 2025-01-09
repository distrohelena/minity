using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;

namespace ps2unity {
    public class TankPlayerComponent : MonoBehaviour {
        public Transform body;

        private void Update() {
            float dt = Time.deltaTime;
            float speed = 15;
            float rotSpeed = 100;

            float ver = Input.GetAxisRaw("Vertical");
            transform.position += transform.forward * ver * speed * dt;

            float hor = Input.GetAxisRaw("Horizontal");
            Vector3 euler = transform.localEulerAngles;
            euler.y += hor * rotSpeed * dt;
            transform.localEulerAngles = euler;
        }
    }
}
