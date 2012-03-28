using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace HomeMediaCenter
{
    [Serializable]
    public class ItemContainerStream : ItemContainer
    {
        public ItemContainerStream(string title, ItemContainer parent, MediaType mediaType) : base(title, parent, mediaType)
        {
        }

        public override void RefresMe(ItemManager manager, IEnumerable<string> directories, HttpMimeDictionary mimeTypes,
            MediaSettings settings, HashSet<string> subtitleExt)
        {
            foreach (Item item in this.childs)
                manager.RemoveItem(item);
            this.childs.Clear();

            foreach (MediaSettingsVideo sett in settings.VideoStreamEncode)
            {
                manager.AddItem(new ItemVideoStream(this, "desktop", "Desktop", sett));

                foreach (string webcam in DSWrapper.WebcamInput.GetWebcamNames())
                {
                    manager.AddItem(new ItemVideoStream(this, "webcam_" + webcam, webcam, sett));
                }
            }
        }
    }
}
