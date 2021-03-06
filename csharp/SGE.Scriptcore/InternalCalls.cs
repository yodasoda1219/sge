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

using SGE.Components;
using SGE.Events;
using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace SGE
{
    [AttributeUsage(AttributeTargets.Class, AllowMultiple = false, Inherited = false)]
    internal sealed class InternalCallsAttribute : Attribute
    {
        public InternalCallsAttribute(string id)
        {
            mID = id;
        }

        public static Type Find(string id)
        {
            var domain = AppDomain.CurrentDomain;
            var assemblies = domain.GetAssemblies();

            foreach (var assembly in assemblies)
            {
                if (assembly.IsDynamic)
                {
                    continue;
                }

                var types = assembly.GetTypes();
                foreach (var type in types)
                {
                    if (!type.IsClass)
                    {
                        continue;
                    }

                    var attribute = type.GetCustomAttribute<InternalCallsAttribute>();
                    if (attribute == null)
                    {
                        continue;
                    }

                    if (attribute.mID == id)
                    {
                        return type;
                    }
                }
            }

            return null;
        }

        private readonly string mID;
    }

    [InternalCalls("core")]
    internal static class CoreInternalCalls
    {
        // application
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetEngineVersion();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void QuitApplication();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetApplicationTitle();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetMainWindow(out IntPtr pointer);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsApplicationEditor();
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsSubsystemInitialized(Subsystem subsystem);

        // window
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AddRef_window(IntPtr window);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void RemoveRef_window(IntPtr window);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void CreateWindow(string title, uint width, uint height, out IntPtr pointer);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern uint GetWindowWidth(IntPtr window);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern uint GetWindowHeight(IntPtr window);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string WindowFileDialog(IntPtr window, DialogMode mode, IReadOnlyList<DialogFilter> filters);

        // scene
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern uint CreateEntity(string name, IntPtr scene);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern uint CreateEntityWithGUID(GUID id, string name, IntPtr scene);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern uint CloneEntity(uint entityID, string name, IntPtr scene);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void DestroyEntity(uint entityID, IntPtr scene);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool FindEntity(GUID id, out uint entityID, IntPtr scene);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void ForEach(Delegate callback, IntPtr scene);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetCollisionCategoryName(IntPtr scene, int index);

        // entity
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern IntPtr AddComponent(Type componentType, Entity entity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool HasComponent(Type componentType, Entity entity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern IntPtr GetComponent(Type componentType, Entity entity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetGUID(uint entityID, IntPtr scene, out GUID guid);

        // guid
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern GUID GenerateGUID();

        // tag component
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTag(IntPtr component, string tag);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetTag(IntPtr component);

        // transform component
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTranslation(IntPtr component, out Vector2 translation);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTranslation(IntPtr component, Vector2 translation);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetRotation(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRotation(IntPtr component, float rotation);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetScale(IntPtr component, out Vector2 scale);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetScale(IntPtr component, Vector2 scale);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetZLayer(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetZLayer(IntPtr component, Entity entity, int zLayer);

        // sprite renderer component
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetColor(IntPtr component, out Vector4 color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetColor(IntPtr component, Vector4 color);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTexture(IntPtr component, out IntPtr texture);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetTexture(IntPtr component, IntPtr texture);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetShader(IntPtr component, out IntPtr shader);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetShader(IntPtr component, Entity entity, IntPtr shader);

        // camera component
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetPrimary(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetPrimary(IntPtr component, bool primary);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern ProjectionType GetProjectionType(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetProjectionType(IntPtr component, ProjectionType type);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetAspectRatio(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetViewSize(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetViewSize(IntPtr component, float viewSize);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetFOV(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetFOV(IntPtr component, float fov);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetOrthographicClips(IntPtr component, out CameraClips clips);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetOrthographicClips(IntPtr component, CameraClips clips);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetPerspectiveClips(IntPtr component, out CameraClips clips);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetPerspectiveClips(IntPtr component, CameraClips clips);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetOrthographic(IntPtr component, float viewSize, CameraClips clips);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetPerspective(IntPtr component, float fov, CameraClips clips);

        // rigid body component
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern BodyType GetBodyType(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetBodyType(IntPtr component, Entity entity, BodyType bodyType);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetFixedRotation(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetFixedRotation(IntPtr component, Entity entity, bool fixedRotation);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetVelocity(Entity entity, out Vector2 velocity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool SetVelocity(Entity entity, Vector2 velocity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetAngularVelocity(Entity entity, out float velocity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool SetAngularVelocity(Entity entity, float velocity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool ApplyForce(Entity entity, Vector2 force, Vector2 point, bool wake);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool ApplyForceToCenter(Entity entity, Vector2 force, bool wake);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool ApplyLinearImpulse(Entity entity, Vector2 force, Vector2 point, bool wake);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool ApplyLinearImpulseToCenter(Entity entity, Vector2 force, bool wake);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool ApplyTorque(Entity entity, float torque, bool wake);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern ushort GetFilterCategory(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetFilterCategory(IntPtr component, ushort category);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern ushort GetFilterMask(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetFilterMask(IntPtr component, ushort mask);

        // box collider component
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetSize(IntPtr component, out Vector2 size);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetSize(IntPtr component, Entity entity, Vector2 size);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetDensity(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetDensity(IntPtr component, Entity entity, float density);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetFriction(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetFriction(IntPtr component, Entity entity, float friction);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetRestitution(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRestitution(IntPtr component, Entity entity, float restitution);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern float GetRestitutionThreashold(IntPtr component);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetRestitutionThreashold(IntPtr component, Entity entity, float threashold);

        // logger
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogDebug(string message);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogInfo(string message);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogWarn(string message);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogError(string message);

        // input
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetKey(KeyCode key);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetMouseButton(MouseButton button);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetMousePosition(out Vector2 position);

        // events
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsEventHandled(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetEventHandled(IntPtr address, bool handled);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetResizeWidth(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetResizeHeight(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetPressedEventKey(IntPtr address, out KeyCode key);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern int GetRepeatCount(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetReleasedEventKey(IntPtr address, out KeyCode key);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetTypedEventKey(IntPtr address, out KeyCode key);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetEventMousePosition(IntPtr address, out Vector2 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetScrollOffset(IntPtr address, out Vector2 offset);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetEventMouseButton(IntPtr address, out MouseButton button);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool GetMouseButtonReleased(IntPtr address);

        // texture2d
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AddRef_texture_2d(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void RemoveRef_texture_2d(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadTexture2D(string path, out IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern TextureWrap GetWrapTexture2D(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern TextureFilter GetFilterTexture2D(IntPtr address);

        // script component
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool IsScriptEnabled(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetScriptEnabled(IntPtr address, bool enabled);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern Script GetScript(IntPtr address, Entity entity);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void SetScript(IntPtr address, Entity entity, Type scriptType);

        // file changed event
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetChangedFilePath(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetWatchedDirectory(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetFileStatus(IntPtr address, out FileStatus status);

        // assets
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern string GetAssetPath(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetAssetType(IntPtr address, out AssetType type);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void GetAssetGUID(IntPtr address, out GUID guid);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern bool ReloadAsset(IntPtr address);

        // prefab
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AddRef_prefab(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void RemoveRef_prefab(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void CreatePrefab(Entity entity, out IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern Entity InstantiatePrefab(IntPtr address, IntPtr scene);

        // shader
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void AddRef_shader(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void RemoveRef_shader(IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadShaderAuto(string path, out IntPtr address);
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LoadShaderExplicit(string path, ShaderLanguage language, out IntPtr address);
    }
}