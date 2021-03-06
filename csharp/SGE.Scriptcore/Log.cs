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

namespace SGE
{
    public static class Log
    {
        public static void Debug(object data) => Debug(data.ToString());
        public static void Debug(string message, params object[] args)
        {
            string formattedString = string.Format(message, args);
            CoreInternalCalls.LogDebug(formattedString);
        }

        public static void Info(object data) => Info(data.ToString());
        public static void Info(string message, params object[] args)
        {
            string formattedString = string.Format(message, args);
            CoreInternalCalls.LogInfo(formattedString);
        }

        public static void Warn(object data) => Warn(data.ToString());
        public static void Warn(string message, params object[] args)
        {
            string formattedString = string.Format(message, args);
            CoreInternalCalls.LogWarn(formattedString);
        }

        public static void Error(object data) => Error(data.ToString());
        public static void Error(string message, params object[] args)
        {
            string formattedString = string.Format(message, args);
            CoreInternalCalls.LogError(formattedString);
        }
    }
}