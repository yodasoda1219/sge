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


namespace SGE
{
    public enum TextureWrap
    {
        Clamp = 0,
        Repeat
    }

    public enum TextureFilter
    {
        Linear = 0,
        Nearest
    }

    public sealed class Texture2D : Asset
    {
        internal Texture2D(IntPtr address, bool addRef = true) : base(address)
        {
            if (addRef)
            {
                InternalCalls.AddRef_texture_2d(mAddress);
            }
        }
        ~Texture2D() => InternalCalls.RemoveRef_texture_2d(mAddress);

        static Texture2D Load(string path)
        {
            IntPtr address;
            InternalCalls.LoadTexture2D(path, out address);

            Texture2D texture = null;
            if (address != IntPtr.Zero)
            {
                texture = new Texture2D(address, false);
            }

            return texture;
        }

        public TextureWrap Wrap => InternalCalls.GetWrapTexture2D(mAddress);
        public TextureFilter Filter => InternalCalls.GetFilterTexture2D(mAddress);
    }
}