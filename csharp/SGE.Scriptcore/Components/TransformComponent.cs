/*
   Copyright 2022 Nora Beda and SGE contributors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

namespace SGE.Components
{
    /// <summary>
    /// A transform component describes an entity's location in a scene.
    /// </summary>
    public sealed class TransformComponent : Component<TransformComponent>
    {
        public Vector2 Translation
        {
            get
            {
                Vector2 translation;
                CoreInternalCalls.GetTranslation(mAddress, out translation);
                return translation;
            }
            set => CoreInternalCalls.SetTranslation(mAddress, value);
        }

        public float Rotation
        {
            get => CoreInternalCalls.GetRotation(mAddress);
            set => CoreInternalCalls.SetRotation(mAddress, value);
        }

        public Vector2 Scale
        {
            get
            {
                Vector2 scale;
                CoreInternalCalls.GetScale(mAddress, out scale);
                return scale;
            }
            set => CoreInternalCalls.SetScale(mAddress, value);
        }

        public int ZLayer
        {
            get => CoreInternalCalls.GetZLayer(mAddress);
            set => CoreInternalCalls.SetZLayer(mAddress, Parent, value);
        }
    }
}