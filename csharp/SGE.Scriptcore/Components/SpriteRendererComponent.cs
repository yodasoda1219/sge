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

using System;

namespace SGE.Components
{
    /// <summary>
    /// A sprite renderer component, as the name suggests, renders a sprite.
    /// </summary>
    public sealed class SpriteRendererComponent : Component<SpriteRendererComponent>
    {
        public Vector4 Color
        {
            get
            {
                Vector4 color;
                CoreInternalCalls.GetColor(mAddress, out color);
                return color;
            }
            set => CoreInternalCalls.SetColor(mAddress, value);
        }

        public Texture2D Texture
        {
            get
            {
                CoreInternalCalls.GetTexture(mAddress, out IntPtr address);

                Texture2D texture = null;
                if (address != IntPtr.Zero)
                {
                    texture = new Texture2D(address);
                }

                return texture;
            }
            set
            {
                IntPtr address = IntPtr.Zero;
                if (value != null)
                {
                    address = value.mAddress;
                }

                CoreInternalCalls.SetTexture(mAddress, address);
            }
        }

        public Shader Shader
        {
            get
            {
                CoreInternalCalls.GetShader(mAddress, out IntPtr address);

                Shader shader = null;
                if (address != IntPtr.Zero)
                {
                    shader = new Shader(address);
                }

                return shader;
            }
            set
            {
                IntPtr address = IntPtr.Zero;
                if (value != null)
                {
                    address = value.mAddress;
                }

                CoreInternalCalls.SetShader(mAddress, Parent, address);
            }
        }
    }
}