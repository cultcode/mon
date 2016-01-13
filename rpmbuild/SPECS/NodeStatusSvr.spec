Name:		NodeStatusSvr
Version:	1.4
Release:	1%{?dist}
Summary:	Node Status Service

Group:		Development/Tools
License:	Commercial
URL:		http://portal.titan.imgo.tv/
Source0:	%{name}-%{version}.tar.gz
BuildRoot:	%(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)

#BuildRequires:	
Requires:	SvrToolKit >= 1.1

%description


%prep
%setup -q


%build


%install
rm -rf %{buildroot}
mkdir -p %{buildroot}/home/Titan/%{name}
cp %{name}        %{buildroot}/home/Titan/%{name}/
cp %{name}.config %{buildroot}/home/Titan/%{name}/


%post
%define LogFile %(grep logfile /home/Titan/%{name}/%{name}.config | awk -F' ' '{printf $3}' | awk -F'=' '{print $2}' | awk -F'"' '{print $2}')
echo "add cron job %{name}"
touch /var/spool/cron/root && \
crontab -l > ~/cron.bak && \
echo >> ~/cron.bak && \
sed -i -e '/%{name}/d' ~/cron.bak && \
sed -i '1i\* \* \* \* \* SvrScheduler.sh /home/Titan/%{name}/%{name} start > /dev/null  2>\&1' ~/cron.bak && \
sed -i '1i0 0 \* \* \* LogTruncate.sh  %{name} %{LogFile} > /dev/null  2>\&1' ~/cron.bak && \
sed -i -e '/^\s*$/d' ~/cron.bak && \
crontab ~/cron.bak
echo "%{name}-%{version} installed into /home/Titan/%{name}"


%postun
echo "remove cron job %{name}"
touch /var/spool/cron/root && \
crontab -l > ~/cron.bak && \
echo >> ~/cron.bak && \
sed -i -e '/%{name}/d' ~/cron.bak && \
sed -i -e '/^\s*$/d' ~/cron.bak && \
crontab ~/cron.bak
echo "%{name}-%{version} uninstalled from /home/Titan/%{name}"


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
%doc
/home/Titan/%{name}/%{name}
/home/Titan/%{name}/%{name}.config


%changelog

