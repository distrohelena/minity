using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using UnityEngine;
using static UnityEngine.GraphicsBuffer;

namespace ps2unity {
    public class TankCameraComponent : MonoBehaviour {
        public TankPlayerComponent player;
        public Transform lookAt;
        public float positionSmoothSpeed = 0.125f;  // The speed at which the camera will catch up with the target's position
        public float rotationSmoothSpeed = 0.125f;  // The speed at which the camera will catch up with the target's rotation

        private Vector3 offset;

        //private void Start() {
        //    offset = transform.position - player.body.position;
        //}

        //private void Update() {
        //    Vector3 transformedOffset = player.body.rotation * offset;

        //    // Calculate the desired position
        //    Vector3 desiredPosition = player.body.position + transformedOffset;

        //    // Use Vector3.Lerp to smoothly move towards the desired position
        //    Vector3 smoothedPosition = Vector3.Lerp(transform.position, desiredPosition, positionSmoothSpeed);

        //    // Update the camera's position
        //    transform.position = smoothedPosition;

        //    transform.LookAt(lookAt);
        //}
    }
}
