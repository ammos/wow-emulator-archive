namespace Server
{
    using System;

    public class DisconnectedEventArgs : EventArgs
    {
        // Methods
        public DisconnectedEventArgs(Mobile m)
        {
            this.m_Mobile = m;
        }


        // Properties
        public Mobile Mobile
        {
            get
            {
                return this.m_Mobile;
            }
        }


        // Fields
        private Mobile m_Mobile;
    }
}

