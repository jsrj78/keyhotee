#include "Contact.hpp"

#include <bts/application.hpp>

#include <fc/reflect/variant.hpp>
#include <fc/log/logger.hpp>

/// QT headers
#include <QBuffer>

const QIcon& Contact::getIcon() const {return icon; }

Contact::Contact( const bts::addressbook::wallet_contact& contact )
: bts::addressbook::wallet_contact(contact)
{
   if( contact.icon_png.size() )
   {
        QImage image;
        if( image.loadFromData( (unsigned char*)icon_png.data(), icon_png.size() ) )
        {
            icon = QIcon( QPixmap::fromImage(image) );
        }
        else
        {
            wlog( "unable to load icon for contact ${c}", ("c",contact) );
        }
   }
   else
   {
      icon.addFile(QStringLiteral(":/images/user.png"), QSize(), QIcon::Normal, QIcon::Off);
   }
}



void Contact::setIcon( const QIcon& icon )
{
   this->icon = icon;
   if( !icon.isNull() )
   {
       QImage image;
       QByteArray byte_array;
       QBuffer buffer(&byte_array);
       buffer.open(QIODevice::WriteOnly);
       image.save(&buffer, "PNG"); // writes image into ba in PNG format
   
       icon_png.resize( byte_array.size() );
       memcpy( icon_png.data(), byte_array.data(), byte_array.size() );
   }
   else
   {
        icon_png.resize(0);
   }
}

QString Contact::getLabel()const
{
   QString label = (first_name + " " + last_name).c_str();
   if( label == " " )
   {
        return dac_id_string.c_str();
   }
   return label;
}

bool Contact::isOwn() const
  {
  bts::application_ptr app = bts::application::instance();
  bts::profile_ptr currentProfile = app->get_profile();

  try
    {
    /// If query for a private key associated to given contact fails, fc::exception is thrown
    auto my_priv_key = currentProfile->get_keychain().get_identity_key(dac_id_string);
    return true;
    }
  catch(const fc::exception&)
    {
    return false;
    }
  }

