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
    [TypedAsset(AssetType.Prefab)]
    public sealed class Prefab : Asset
    {
        private static IntPtr Create(Entity entity)
        {
            CoreInternalCalls.CreatePrefab(entity, out IntPtr address);
            return address;
        }

        public Prefab(Entity entity) : base(Create(entity)) { }
        internal Prefab(IntPtr address) : base(address)
        {
            CoreInternalCalls.AddRef_prefab(address);
        }

        ~Prefab() => CoreInternalCalls.RemoveRef_prefab(mAddress);

        public Entity Instantiate(Scene scene)
        {
            return CoreInternalCalls.InstantiatePrefab(mAddress, scene.mNativeAddress);
        }
    }
}